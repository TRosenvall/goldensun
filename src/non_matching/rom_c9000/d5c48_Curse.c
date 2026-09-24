/* Anim_Curse -- NON-MATCHING, 58 ENCODINGS OF 233.  SIZE EXACT (524 bytes),
 * INSTRUCTION COUNT EXACT (233), AND ALL 19 RELOCATIONS SIT AT IDENTICAL
 * OFFSETS -- the only relocation difference is the SYMBOL `_call_via_r7`
 * (ROM) against `_call_via_r4` (ours), at both call sites.  Every offset
 * matching means there is NO length shift anywhere in the stream: all 60
 * differences are register choices and same-length permutations.
 * FRAME `sub sp, #0x20` against the ROM's `sub sp, #0x24` -- one 4-byte
 * spill slot short (see THE FRAME below).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/d5258_Curse.c \
 *     asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s --func Anim_Curse
 * ONE function; no .rodata; no verification shim.  Progression in batch 284:
 * 223 -> 178 -> 170 -> 126 -> 102 -> 60 -> 58.
 *
 * ================================================================
 * LOAD-BEARING, each measured on this function
 * ================================================================
 *
 * `base` PINNED TO r8.  Unpinned, gcc gives `base` r7 and the &iwram_3001ef0
 * pointer r8 -- the ROM's roles exactly reversed (223).  The pin is the one
 * that works here; pinning the TABLE pointer to r7 instead makes gcc coalesce
 * `base` into r7 as well and MISCOMPILE (`ldr r1,[r7,#24]` reads base[0x18]
 * where the source says tbl[6]) -- do not use that spelling even though it
 * screens at 233/524.
 *
 * BOTH LOOP WALKERS ARE EXPLICIT POINTER LOCALS, NOT STRENGTH-REDUCED givs.
 * With `base` pinned, loop.c will not build a giv over `base + const + i*0x1c`
 * (the hard reg sits inside the MEM address), and the indexed spelling
 * `((Unit *)(base + (0xe1<<7)))[i].rnd` costs 6 insns per iteration.  Written
 * as walkers they reproduce the ROM exactly, INCLUDING ITS ASYMMETRY: loop 1
 * is `int *p = (int *)(base + 0x7098); ... *p = v; p += 7;` (the +0x18 folded
 * into the pool word, `ldr r5,=0x7098 / add r5,r8`), loop 2 is
 * `Unit *u = (Unit *)(base + (0xe1<<7)); ... u->rnd ... u++;` (the +0x18 left
 * in the load, `mov r0,#0xe1 / lsl #7 / add r0,r8 / str r0,[sp,#8]`).
 * THAT ASYMMETRY IS THE EVIDENCE THEY ARE TWO DIFFERENT SOURCE POINTERS --
 * a giv would fold the field offset in both places.
 * 170 -> 126 came with these plus the next item.
 *
 * A SECOND `State **` LOCAL ASSIGNED BEFORE THE INNER-LOOP GUARD.  The ROM
 * keeps `base + 0x7828` in r11 across the inner loop and re-materialises it
 * everywhere else.  `sl = (State **)(base + 0x7828); if ((*sl)->f14 != 0) {
 * do ... while (i != (*sl)->f14); }` gets `ldr r2,=0x7828 / add r2,r8 / ldr /
 * cmp / beq` for the guard and then `mov r11, r2` in the preheader -- gcse
 * hoisting the address it already computed for the guard.  The PROLOGUE uses
 * a separate `slot` local and the two loop-1 sites use the RAW expression:
 * writing `sl` everywhere collapses them into one pseudo and one register.
 *
 * PIN THE THREE COUNTERS TOO: `sl` r11, `i` r10, `frame` r9.  All three are
 * needed together (126 -> 102); `sl` alone is 169, `i`+`frame` alone is inert
 * at 126, `sl`+`i` is 120.  Pinning `sl` alone frees r7 and gcc immediately
 * moves `i` into it, which is why the partial sets lose.
 *
 * THE OPERAND-ORDER SWAP ON A `base + variable` ADDRESS, AND IT IS WORTH 42.
 * `blit(ctx, base + n * 0xc0, ...)` emits `lsl r3,r0,#1 / add r3,r3,r0 /
 * lsl r3,r3,#6 / mov r4,r8 / add r1,r4,r3` -- gcc expands `plus(base, mult)`
 * with the hard reg first, so the multiply cannot use the argument register as
 * its subtarget and a copy of r8 is needed.  Writing it
 * `(u8 *)(n * 0xc0 + (int)base)` puts the MULT first, the chain expands
 * straight into r1, and the add becomes the ROM's destructive `add r1, r8`.
 * 102 -> 60 IN ONE EDIT.  Note this is the lever Tackle's park recorded as
 * INERT (`ix + (char *)Data_ede48`): IT IS NOT INERT, IT IS CONDITIONAL ON THE
 * ADDRESS HAVING A NON-CONSTANT OFFSET, and it was a NEGATIVE here (133) until
 * the three counter pins were in place.  Measure it last, not first.
 *
 * `REG_BLDCNT`, NOT `REG_BLDALPHA`.  The ROM writes `add r2,#0x30` from
 * REG_BG2PA (0x4000020), i.e. 0x4000050; include/gba/io.h puts BLDALPHA at
 * 0x4000052 and BLDCNT at 0x4000050.  With BLDALPHA the immediate is #50
 * against the ROM's #0x30 and it is the only wrong instruction in the
 * prologue -- a one-register-off bug that reads as scheduling noise.
 *
 * ALSO KEPT: `arg = 0x90; arg <<= 3;` as its own statements (the recorded
 * StartTask lever); the redundant inner `if (frame >= th)` around the second
 * blit, which the ROM has and gcc does not thread away because calls
 * intervene; `pos.y -= 0x10;` before `blit(..., pos.y - 4, ...)`, which combine
 * folds to the ROM's `sub r3, #0x14` off the PRE-store value.
 *
 * ================================================================
 * THE 58, in four named components
 * ================================================================
 *
 * SCRATCH-REGISTER NAMING, about 16, AND IT IS THE REG_ALLOC_ORDER CLASS.
 * THE ROM NEVER TOUCHES r4 IN THIS FUNCTION -- not in the push mask, not once
 * in 233 instructions -- while gcc reaches for it for every short-lived
 * temp (`movs r4,#0 / mov r9,r4` against the ROM's `movs r0,#0 / mov r9,r0`,
 * `mov r4,sl` against `mov r0,sl`, and the two blit reloads).  That is
 * HANDOFF.md's `REG_ALLOC_ORDER` hypothesis showing up as a whole park:
 * the ROM behaves as though r4 came after r0-r3 AND after r5-r7.
 * MEASURED NEGATIVE, and it kills the obvious workaround: `-fcall-saved-r4`
 * does NOT emulate it -- gcc still picks r4 and merely adds it to the push
 * mask, so the prologue changes from b5e0 to b5f0 and the score goes 223 -> 228.
 * This stem must stay on the default `-fcall-used-r4`.
 *
 * THE CONSTANT 0x10 IS SHARED ACROSS THE TWO INDIRECT CALLS, about 12.
 * The ROM materialises `movs r0,#16` separately at each blit; gcc keeps one
 * copy in r7, which both costs r7 (hence `_call_via_r4` for the two reloads,
 * the only relocation difference) and rotates both argument-setup windows.
 * EVERY ESCAPE ON FILE WAS TRIED AND ALL ARE NEGATIVE: the recorded
 * "pinned call-clobbered register breaks cse1's constant sharing" form
 * (`register int w __asm__("r0")`) at the first site is 133, at the second 126,
 * at both 126; a block-scoped `int w = 0x10;` at both sites 142; a
 * function-scope `int w` declared between `ctx` and `blit` 111; and
 * `-fno-gcse` 139.  Tackle's cse1 route does not reach this site.
 *
 * ONE 4-BYTE SPILL SLOT MISSING, 6.  The ROM's frame is 0x24 and reads
 * `pos(0x18..0x23), ctx(0x14), NOTHING(0x10), blit(0x0c), u(0x08)`, outgoing
 * args 0x00-0x07.  Ours is 0x20 with ctx at 0x10 -- correct order, one slot
 * short, which is the recorded "written-never-read slot" signature.  The 0x10
 * slot has NO instruction touching it anywhere in the ROM.
 * WHAT IT IS NOT, all measured inert at 126: a dead `int hole;` declared
 * between ctx and blit and assigned once (flow1 deletes the store AND the
 * slot -- so THE RECORDED "written-never-read slot" LEVER DOES NOT WORK FOR A
 * SCALAR IN gcc-2.96, confirming the array-element half of that rule);
 * a `vec3_t *pp = &pos;` pointer local (gcc rematerialises `add r5,sp,#0x18`
 * and allocates no slot); `sl` declared between ctx and blit; `pos` declared
 * last.  A 4-byte ARRAY cannot produce it either: addressables are allocated
 * from the TOP of the frame, so an array either lands above `pos` or pushes
 * `&pos` down to 0x14, and the ROM has `add r5, sp, #0x18`.
 * The remaining explanation is a pseudo that reload gave a slot and then did
 * not need -- not reachable from C.
 *
 * SCHEDULING PERMUTATIONS, about 14, in two windows, all same-length.
 * @31 IS CLOSED: ROM puts `ldr r0,=Task_BlitAnim` before `lsl r1,#3` and gcc
 * fills r0 last; a pinned r0 assigned as its OWN STATEMENT ahead of the shift
 * restores the ROM's order (60 -> 58).  Same edit closed BaseAnim_Tackle's @75
 * and was the last two encodings of the MATCHING Anim_Vine.  The two left:
 *   @61  the `i = 0` pair (`movs r3,#0 / mov sl,r3`) is interleaved INTO the
 *        `sl = base + 0x7828` computation in the ROM, and the `mov r11,r2`
 *        that finishes it is deferred to the preheader; ours emits both
 *        contiguously.  Swapping the two source statements is 139, worse --
 *        this is the "assign the base+K pointer LAST" lever RUNNING OUT, the
 *        same wall Tackle's four windows hit.
 *   @216 the frame-loop exit test: ROM interleaves `add r9,r1` (frame++) into
 *        the limit computation; ours emits frame++ first.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies
 * (all three of -fno-gcse, -fno-schedule-insns2 and -fno-rerun-cse-after-loop
 * measured, 139 / 116 / 60-unchanged).
 *
 * NEXT: the 0x10 constant window.  It is worth 12 encodings AND the last
 * relocation difference, and it is a CSE question rather than a scheduling
 * one, which makes it the only component here with an unexhausted mechanism.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int f0, f4, f8, fc, f10, f14, rnd;
} Unit;

extern u8 *iwram_3001ef0;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int Random(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Curse(void *context)
{
    vec3_t pos;
    void *ctx;
    DrawFn blit;
    char **tbl;
    register u8 *base __asm__("r8");
    State **slot;
    register State **sl __asm__("r11");
    register int i __asm__("r10");
    register int frame __asm__("r9");
    int arg;

    tbl = (char **)&iwram_3001ef0;
    ctx = (void *)tbl[0];
    base = (u8 *)tbl[-1];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(1);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    LoadVFXFile(FILE_7a, base, 1, 1);
    if ((*slot)->f4 == 1) {
        REG_BG2X = 0xffff9000;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    blit = (DrawFn)tbl[6];
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    i = 0;
    if ((*slot)->f14 != 0) {
        int *p = (int *)(base + 0x7098);
        do {
            *p = Random() & 0x3f;
            i++;
            p = (int *)((char *)p + 0x1c);
        } while (i != (*(State **)(base + 0x7828))->f14);
    }
    frame = 0;
    if ((*(State **)(base + 0x7828))->f14 * 0x20 + 0x20 != 0) {
      do {
        if (frame == 0x20) {
            _Func_80bd7dc(0);
        }
        sl = (State **)(base + 0x7828);
        i = 0;
        if ((*sl)->f14 != 0) {
            Unit *u = (Unit *)(base + (0xe1 << 7));
            do {
                int th = i * 0x10;
                if (frame == th) {
                    _PlaySound(0x8f);
                }
                if (frame >= th && frame < th + 0x48) {
                    GetBattleActorPos2((*sl)->ids[i], &pos);
                    if ((*sl)->f4 == 1) {
                        pos.x -= 0x70;
                    }
                    pos.y -= 0x10;
                    blit(ctx, base + (0xd8 << 3), pos.x - 8, pos.y - 4, 0x10, 0x14);
                    if (frame >= th) {
                        int n = ((frame - th + u->rnd) / 6) % 9;
                        blit(ctx, (u8 *)(n * 0xc0 + (int)base), pos.x - 8, pos.y - 0x10, 0x10, 0xc);
                    }
                }
                i++;
                u++;
            } while (i != (*sl)->f14);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      } while (frame != (*(State **)(base + 0x7828))->f14 * 0x20 + 0x20);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
