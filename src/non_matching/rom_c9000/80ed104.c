/* Anim_UndeadSword  [rom_c9000]  --  asm/rom_c9000/rom_ece7c_c_c.s
 *
 * NON-MATCHING: 204 encodings of 336 differ (objcmp).
 * Size 768 against the ROM's 772 (-4); 334 instructions against 336 (-2).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/ece7c_UndeadSword.c \
 *     asm/rom_c9000/rom_ece7c_c_c.s --func Anim_UndeadSword
 *
 * ONE RELOCATION DIFFERENCE IS A SPELLING, NOT A DEFECT: the ROM's `.s` writes
 * `bl _call_via_r12` where gcc emits `bl _call_via_ip`.  src/lib/call_via.s:29
 * defines `_call_via_ip` with the comment "gcc emits `ip` for r12", so the two
 * name the same veneer; objcmp compares relocation SYMBOL NAMES and therefore
 * always reports RELOCATIONS differ on this function.  Check the rest of the list
 * against it, not the headline.
 *
 * THE .s IS 2-of-2 PLUS A .rodata TAIL.  Both labels there belong to THIS function
 * (see ece7c_FullScreenSlash.c for the full datacheck output and the byte values);
 * they are `unsigned short[7]`, confirmed by the ROM's `ldrh rX,[r3,r2]` with
 * r2 = idx*2, and NEITHER is `.global`, so a split must export both.
 *
 * ================================================================
 * WHAT IS RIGHT
 * ================================================================
 *
 * FRAME `sub sp, #0x2c` IS EXACT AND EVERY SLOT LANDS ON THE ROM'S OFFSET:
 *   0x08 t | 0x0c slot2 | 0x10 fp | 0x14 ctx | 0x18-0x1c fns[2] | 0x20-0x28 pos
 * Reaching that needed three separate readings, each worth measuring:
 *
 * THE ROM HAS **TWO** `base + 0x7828` COMPUTATIONS, NOT ONE SPILL.  Uses 1-3
 * (`*slot = context`, BuildDraw2DFuncs, GetBattleActorPos3) share r6; the loop
 * reads a second one out of sp+0xc built by its own `ldr r1,=0x7828 / add r1,r10`.
 * One `slot` local gives one pointer spilled at sp+0x10 and the whole slot map
 * shifts.  Two locals -- `slot` for the head, `slot2` assigned beside
 * `*(int *)(base + 0x77a8) = 8` -- and gcse does NOT unify them.  311 -> 210.
 *
 * `void *fns[2]` WITH A SEPARATE `DrawFn *fp = fns;` -- the d9ab8_StatDown.c
 * reading, confirmed here on a second function.  The ROM's first
 * BuildDraw2DFuncs takes the array address directly (`mov r2,sp / add r2,#0x18`)
 * and the second takes the pointer (`ldr r1,[sp,#0x10]`); `fns[0]` is then read as
 * a direct frame ref `ldr r4,[sp,#0x18]` at the second blit site.  That only
 * happens with BOTH an array and a pointer to it.
 *
 * DECLARATION ORDER SET THE 0xc/0x10 PAIR, and the direction is batch 290's:
 * LATER DECLARATION TAKES THE LOWER SLOT.  `slot2` declared AFTER `fp` puts slot2
 * at 0xc and fp at 0x10, the ROM's way round; the reverse order swaps them.
 *
 * `fns[0] = iwram_3001f08;` then `fns[0](...)` -- the ROM reads the VALUE at
 * iwram_3001f08 (`ldr r4,=iwram_3001f08 / ldr r4,[r4]`) and STORES it into fns[0]
 * before calling through it, overwriting what BuildDraw2DFuncEx put there.  Note
 * this file's OTHER function reaches the same address as `gPtrs[0x2e]`
 * (gPtrs = 0x03001E50, +0xb8 = 0x03001F08); both spellings are correct, 0x210
 * bytes apart.
 *
 * THE OUTER LOOP IS A `goto` LOOP -- `beq .Led3ba / b .Led232`, a conditional exit
 * followed by an UNCONDITIONAL back edge, which is exactly the shape
 * `expand_end_loop` (stmt.c:2340-2551) leaves when it finds no conditional jump to
 * `end_label`.  Nothing in its body is hoisted: `0x77a8` is rebuilt inside it
 * although it was also built outside, and so are `0x7828`, `0x7824` and `0xe1<<7`.
 * The two inner loops ARE real `do ... while` loops (conditional back edges) and do
 * get preheaders.
 *
 * `sin(ang) * mag`, NOT `mag * sin(ang)` -- thumb `mul rd,rs` puts operand 0 in the
 * destination and the ROM's `mov r3,r6 / mul r3,r0` wants mag there, which is what
 * the call-first spelling produces.  The multiply-operand lever firing the same way
 * as on BaseAnim_StatDown.
 *
 * ONE TABLE POINTER: `tbl = iwram_3001eec; pp = tbl; base = *pp++; ctx = *pp;
 * gfx = tbl[2];` for the ROM's `ldr r2,=iwram_3001eec / mov r3,r2 / ldmia r3!,{r1}
 * ... ldr r5,[r2,#8]` -- one pool load, kept in r2 and re-indexed.  Reading
 * `iwram_3001eec[2]` separately costs a second pool load.
 *
 * SPLIT THE PARTICLE POINTER PER REGION: the ROM walks base+0x7080 in r7 in the
 * first loop and in r5 in the second, so `p` and `p2` are two locals (the same
 * rule that closed Func_80cc960 in this batch).
 *
 * `src = base + (0x8c << 3) + Leef88[idx];` BEFORE `wd = Leef96[idx];` -- the ROM
 * interleaves `add r1,r10 / add r1,r3` between the two `ldrh`s, so the Leef88 read
 * must be the earlier statement; written the other way round the two pool words and
 * both loads swap.
 *
 * `mode = 3; if (len > 0x50) { len = 0x50; mode = 2; }` measured 5 encodings better
 * than the ROM's OWN `if/else` shape (204 against 209).  BE CAREFUL WITH THAT
 * NUMBER: the ROM really does emit `mov r7,#2 / b / mov r7,#3`, and the gain is
 * register-rename noise, not structure.  The if/else form is in the history as t2.c.
 *
 * ================================================================
 * THE BLOCKER: THREE INNER-LOOP REGISTERS ARE A CYCLIC PERMUTATION
 * ================================================================
 *
 * Instruction counts are 334 against 336 and the two missing instructions are both
 * in the `i == 0xa` block; everything else is a rename. The ROM assigns
 *     r5 len | r6 x | r7 mode | r8 k | r9 i | r10 base | r11 0x70
 * and ours
 *     r5 len | r6 k | r7 x | r8 mode | r9 i | r10 base | r11 0x70
 * -- the same seven values, the same count, three of them rotated.  REG_ALLOC_ORDER
 * puts r7 ahead of r8, so the ROM's `mode` outranks its `k`; ours is the reverse.
 * Every ref of `k` is at loop depth 2 and `global.c`'s `allocno_compare` weights by
 * depth, so `k` outranking `mode` is what the arithmetic gives -- the ROM needs `k`
 * BELOW a value whose refs are all at depth 1, which its own instructions do not
 * supply.  This is the same shape as batch 290's `Func_80c1afc` finding and should
 * be read the same way: state the arithmetic, do not list spellings.
 *
 * INERT, all measured on this file: `int` on _SetBattleActorKnockback; the inner
 * loop as a `goto` loop (227 of 336, two instructions short); `x` as the
 * strength-reduced giv `k * 0xe + 0x32` instead of an explicit local (exactly 204,
 * so the giv/local question is not the lever here); `x = 0x32;` before `k = 0;`;
 * moving `mode`'s or `k`'s declaration.  `int StartTask` is worth exactly 1.
 *
 * ALSO STILL OPEN, and both follow the permutation: the ROM materialises the
 * constant 8 TWICE inside the `i == 0xa` block (`mov r4,#8 / str r4,[r3]` then a
 * fresh `mov r2,#8 / str r2,[sp]`) where ours reuses one register -- because the
 * ROM's first 8 lands in r4, which is CALL-CLOBBERED under -fcall-used-r4 and so
 * dies across the two intervening calls, while ours lands in callee-saved r5 and
 * `reload_cse_regs` reuses it.  And `_SetBattleActorKnockback`'s r1 is filled
 * before its r0 in the ROM.
 *
 * No new symbol is warranted.  No per-file Makefile flag override applies.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x;
    int y;
    int z;
    int vx;
    int vy;
    int vz;
    int t;
} Part;

extern int *iwram_3001eec[];
extern DrawFn iwram_3001f08;
extern unsigned short Leef88[] __asm__(".Leef88");
extern unsigned short Leef96[] __asm__(".Leef96");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int id, void *fns);
extern void BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void gfree(int tag);
extern void Func_80e3908(Part *p, int a, int b);
extern void UpdateScreenShake(int a, int b);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);

void Anim_UndeadSword(void *context)
{
    int **tbl;
    int **pp;
    unsigned char *base;
    void *ctx;
    unsigned char *gfx;
    State **slot;
    DrawFn fns[2];
    DrawFn *fp;
    State **slot2;
    vec3_t pos;
    Part *p;
    Part *p2;
    int i;
    int j;
    int k;
    int t;
    int mag;
    int ang;
    int len;
    int mode;
    int x;

    tbl = iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    gfx = (unsigned char *)tbl[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    REG_BG2PA = 0x100;
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_51, base, 1, 1);
    LoadVFXFile(FILE_c0, base + (0x8c << 3), 1, 0);
    fp = fns;
    BuildDraw2DFuncs((*slot)->f4, fns);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    GetBattleActorPos3((*slot)->ids[0], &pos);
    REG_BG2X = (0x40 - pos.x) << 8;
    j = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        mag = (Random() & 0x1ff) + 0x80;
        ang = Random() & 0xffff;
        p->x = 0x80 << 15;
        p->y = 0xe0 << 15;
        p->vx = (sin(ang) * mag) >> 8;
        p->vy = (cos(ang) * mag) >> 9;
        p->t = Random() & 7;
        j++;
        p++;
    } while (j != 0x10);
    *(int *)(base + 0x77a8) = 8;
    slot2 = (State **)(base + 0x7828);
    t = 0;
    i = 0;
again:
    {
        if (i == 0xa) {
            *(int *)(base + 0x77a8) = 8;
            _Func_80bd7dc(0xd4);
            _SetBattleActorKnockback((*slot2)->ids[0], 0);
            Func_80d6888((*slot2)->ids[0], 7, 5, 0, 8);
        }
        if (i > 7) {
            if (i <= 0x1f) {
                len = i * 12 - 0x60;
            } else {
                len = (0x88 << 1) - t;
            }
            if (len > 0) {
                mode = 3;
                if (len > 0x50) {
                    len = 0x50;
                    mode = 2;
                }
                k = 0;
                x = 0x32;
                do {
                    if (k == 0) {
                        BuildDraw2DFuncEx(0x2e, 7, 7, 3, mode);
                    } else {
                        BuildDraw2DFuncEx(0x2e, 7, 7, 7, mode);
                    }
                    fns[0] = iwram_3001f08;
                    fns[0](ctx, base, x, 0x70 - len, 0xe, len);
                    gfree(0x2e);
                    k++;
                    x += 0xe;
                } while (k != 2);
            }
        }
        BuildDraw2DFuncs((*slot2)->f4, fp);
        j = 0;
        p2 = (Part *)(base + (0xe1 << 7));
        do {
            if (i >= j / 2 + 8) {
                int n = p2->t;
                if (n <= 0x1c) {
                    int idx = n / 3;
                    int px = *(short *)((char *)p2 + 2);
                    int py = *(short *)((char *)p2 + 6);
                    unsigned int wd;
                    unsigned char *src;

                    if (idx > 6) {
                        idx = 6;
                    }
                    src = base + (0x8c << 3) + Leef88[idx];
                    wd = Leef96[idx];
                    fns[0](ctx, src, px - (wd >> 1), py - (wd >> 1), wd, wd);
                    p2->t += 1;
                    Func_80e3908(p2, 0x3e, -0x2000);
                }
            }
            j++;
            p2++;
        } while (j != 0x10);
        gfree(0x2f);
        gfree(0x2e);
        if (i <= 7) {
            UpdateScreenShake(2, 2);
        } else {
            UpdateScreenShake(0x10, 0x10);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        i++;
        t += 6;
    }
    if (i != 0x36) {
        goto again;
    }
    StopTask(Task_BlitAnim);
    AnimEnd();
}
