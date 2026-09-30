/* BaseAnim_Spasm -- 0x080ceff8, the shared implementation behind the three
 * wrappers Anim_Spasm (mode 0), Anim_Berserk (mode 1) and Anim_Recovery
 * (mode 2), which are ALREADY ELEVATED as rom_cefd4_c_b / _b / _c_c_b and each
 * consist of `mov r1, #K; bl BaseAnim_Spasm`.  Those three files fix the
 * signature: `void BaseAnim_Spasm(void *context, int mode)`.
 *
 * NON-MATCHING: 2 encodings of 291 differ (objcmp).
 *
 * AND IT IS A TRUE DISTANCE.  SIZE agrees (680 bytes, objcmp prints no SIZE
 * line), the instruction count agrees (ref 291, ours 291) and ALL 38
 * RELOCATIONS AGREE IN TYPE, SYMBOL AND OFFSET -- including the three .rodata
 * label references, whose offsets pin the literal-pool word order.  Every
 * number quoted below that is NOT accompanied by a matching count is flagged;
 * a difference count is not a distance unless size and instruction count both
 * match.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/80ceff8.c \
 *     asm/rom_c9000/rom_cefd4_c_c_c.s --func BaseAnim_Spasm
 *
 * ================================================================
 * THE SPLIT SHAPE -- THREE DATA LABELS, NONE OF THEM `.global`
 * ================================================================
 *
 * asm/rom_c9000/rom_cefd4_c_c_c.s holds exactly ONE function plus a `.rodata`
 * section of three blobs, and BaseAnim_Spasm reads ALL THREE:
 *
 *   .Lee096   0xee096..0xee09c   6 bytes   read `ldrh r1,[r4,r5]`, r5 = n*2  -> u16[3]
 *   .Lee09c   0xee09c..0xee09f   3 bytes   read `ldrb r4,[r2,r6]`           -> u8[3]
 *   .Lee09f   0xee09f..0xee0a2   3 bytes   read `ldrb r0,[r3,r6]`           -> u8[3]
 *
 * ALL THREE CROSS THE BOUNDARY IN ONE DIRECTION: out of the residual `.s`,
 * which keeps the data, into this `.c`, which consumes them as externs.
 * NOTHING crosses the other way -- a tree-wide grep finds no reference to any
 * of the three outside rom_cefd4_c_c_c.s.
 *
 * NONE of the three is currently `.global`, so all three need a `.global` line
 * added to the residual `.s`.  `tools/datacheck.py` prints NO EXPORTS line for
 * this file for exactly that reason: ITS EXPORTS LINE LISTS ONLY WHAT IS
 * ALREADY `.global`, so it is NOT the set a split requires.  Read the function
 * body, not datacheck, to find the labels a split must export.
 *
 * DO NOT EMIT THE BLOBS FROM C.  They sit in the MIDDLE of a long cross-file
 * .rodata run: asm/rom_c9000/rom_ceb30_c_c_c_c.s supplies 0xee090..0xee096
 * immediately before and asm/rom_c9000/rom_cf2a0_c_c.s supplies
 * 0xee0a2..0xee0aa immediately after.  Moving them out of the middle would move
 * their addresses.  This is the Anim_Vine case (src/rom_c9000/rom_dd2ac_c_c_b.c)
 * and NOT the Anim_Break case (src/rom_c9000/rom_d82b0_b.c, whose blob was the
 * only object in its slot).  So the landing is:
 *
 *   src/rom_c9000/rom_cefd4_c_c_c_b.c   this function
 *   asm/rom_c9000/rom_cefd4_c_c_c_c.s   `.section .rodata`, three `.global`
 *                                       lines, then the three labels unchanged
 *
 * Template for the residual file: asm/rom_c9000/rom_ceb30_c_c_c_c.s is exactly
 * this shape already (one `.global` + one blob); the three-`.global` precedent
 * is asm/rom_c9000/rom_cc5d8_c_c.s.  With the blobs left in asm, this candidate
 * emits no .rodata of its own and objcmp's SIZE line stays clean -- no
 * .rodata-from-C false positive to discount.
 *
 * ================================================================
 * THE BLOCKER: A sched2 READY-LIST RANK, AND THE TRACE SAYS SO
 * ================================================================
 *
 * The two differing encodings are ONE TRANSPOSITION of adjacent independent
 * instructions in the StartTask block:
 *
 *     ROM   118 adds r5,#188   119 str r6,[r3]   120 ldr r5,[r5]   121 movs r3,#50   122 add r2,r8
 *     OURS  118 adds r5,#188   119 str r6,[r3]   120 add r2,r8     121 movs r3,#50   122 ldr r5,[r5]
 *
 * `add r2,r8` completes `base + 0x7784`; `ldr r5,[r5]` is the `gPtrs[0xbc/4]`
 * read.  Everything else in the block, including `str r3,[r2]` at 124 and the
 * `str r5,[sp,#0xc]` spill at 127, is already the ROM's.
 *
 * PASS: `sched2` (haifa `rank_for_schedule`).  Confirmed, not assumed:
 * `-fno-schedule-insns2` gives 250 differing AND drops the count to 288, so the
 * ROM's shape REQUIRES sched2 -- the ROM is itself scheduled and this is a rank
 * difference inside it, not a residue an earlier pass can place.
 *
 * READ FROM `.23.sched2` WITH `-fsched-verbose=6` (the documented probe for this
 * class).  The competitors are uid 291 (`r2=r2+r8`) and uid 303 (`r5=[r5]`):
 *
 *     ;; Ready list (t = 91):  312  884  303  291  284   -> picks 284   (ROM agrees)
 *     ;; Ready list (t = 92):  312  293  884  303  291   -> full stall
 *     ;; Ready list (t = 93):  312  293  884  303  291   -> picks 291   (ROM picks 303)
 *
 * The list is rank-sorted with the winner LAST, and 291 outranks 303 at every
 * cycle both are ready.  The rank is NOT a plain INSN_LUID tie: uid 301
 * (`r5=r5+0xbc`) wins t=90 over the LOWER-uid 284 and 291, and uid 293
 * (`r3=0x32`, LOWER than 303) ranks BELOW 303 -- so the second criterion, not
 * insn order, is deciding, and 291/293 tie on priority while straddling 303.
 *
 * WHY SOURCE ORDER CANNOT REACH IT.  `ldr r5,[r5]` depends on `adds r5,#188`,
 * and insn order follows statement order, so lowering the load's uid below 291
 * necessarily lowers the address add's uid too -- and the add then schedules
 * EAGERLY at 116 instead of the ROM's 118, carrying the load past `str r6,[r3]`
 * as well.  Measured on exactly that move (d1 read placed between the two
 * stores): the pair DOES flip, and the block lands one slot early instead --
 * 10 differing, count still 291.  Splitting the read into an address statement
 * and a load statement does not separate them either (11 and 10).  Reaching the
 * ROM would need the load ranked above `add r2,r8` WITHOUT moving `adds r5,#188`,
 * i.e. an anti-dependence on r2 or r8 that the instruction set does not contain
 * -- the Func_8077f70 shape of proof rather than an exhausted search.
 *
 * ================================================================
 * ELEVEN LOAD-BEARING CONSTRUCTS, each a SINGLE DROP from this file (baseline 2)
 * ================================================================
 *
 * Counts marked (*) are NOT distances -- the instruction count moved too, as noted.
 *
 *  (a) 275 (*) count 293  Each of the three `Func_8001af8` sites REBUILDS
 *      `0xa0 << 19` in a PINNED r0.  Left as three identical
 *      `(volatile u16 *)(0xa0 << 19)` expressions, cse2 unifies them into one
 *      callee-saved pseudo (r6) and copies `mov r0,r6` at each site; the ROM has
 *      `mov r0,#0xa0 / lsl r0,#19` at all three.  r0 IS CALL-CLOBBERED, which is
 *      why the ROM must rebuild -- the pin reproduces that by construction.
 *      TWO SPELLINGS REACH IT AND THE SPLIT IS PER SITE.  Sites 2 and 3 take the
 *      batch's separate-inline-bodies lever -- each its OWN `static inline` with
 *      the literal written INSIDE the body -- and that is what this file uses, at
 *      2, with no pin.  SITE 1 DOES NOT: an inline body there gives 4, because
 *      site 1 is the one whose argument setup the ROM interleaves with
 *      `ldr r6,=Func_8001af8` and `adds r5,#128`, and only a pinned r0/r1 pair
 *      orders those.  So the inline-bodies lever and the pin are interchangeable
 *      on a PLAIN repeated-constant site and are NOT interchangeable where the
 *      site's argument setup is also interleaved -- prefer the inline body, which
 *      costs no shim, and fall back to the pin only where ordering is at stake.
 *  (b) 273 (*) count 288  `(*(State **)(base + 0x7828))->ids[0]` written INLINE
 *      inside the frame loop, NOT the `slot` local used in the prologue.  The
 *      ROM keeps `r7 = base+0x7828` for the prologue's two uses, then REUSES r7
 *      as the frame counter and RE-COMPUTES the address in the loop.  Reusing
 *      `slot` keeps it live and costs r7.  ("Split reused locals per region.")
 *  (c) 209 (*) count 288  `REG_BG2X = x << 8;` written in BOTH arms of the mode
 *      test.  The ROM loads REG_BG2X's address in each arm and shares only the
 *      `lsl r3,r0,#8 / str r3,[r2]` tail, which jump.c cross-jumps back into one
 *      block -- the BuildDraw2DFuncs "duplicated ROM code means duplicated
 *      source" rule.  Sharing the store at the join is 8 bytes and one
 *      instruction short.
 *  (d)  91 (*) count 289  `n = frame / 4` computed BETWEEN the `frame < 0x20`
 *      and `frame < 0x1c` guards, with two nested `if`s -- not after a single
 *      merged guard.
 *  (e)  14  `Lee096`, `Lee09c` and `Lee09f` are DIRECT ARRAY READS OFF THEIR
 *      SYMBOLS, with NO named pointer local for any of them.  A named
 *      `u8 *h = Lee09f;` before the loop does NOT put Lee09f in r11 -- loop.c
 *      hoists all three and the allocator picks Lee09c instead, which also
 *      reorders the region-3 pool words and breaks three relocation offsets.
 *      With no local, gcc hoists Lee09f itself, exactly as the ROM does.  This
 *      is the recorded "gPtrs IS A DIRECT READ, NOT A NAMED LOCAL" result
 *      (src/rom_c9000/rom_ceb30_c_c_c_b.c) generalising to .rodata symbols.
 *  (f)  10  `BuildDraw2DFuncEx` declared to RETURN A VALUE, not `void` -- the
 *      same lever and the same direction as BuildDraw2DFuncs, where `void`
 *      emits `mov r0,#0x2e` one slot too early.
 *  (g)   6  `data += 0x80` as a WALKING POINTER before `DecompressLZ(data, base)`,
 *      not `DecompressLZ(data + 0x80, base)`.  One pseudo updated in place lets
 *      the ROM reuse r5 destructively and schedule `adds r5,#128` into the
 *      argument setup ahead of the call.  (cf2a0_Revive's recorded lever 4.)
 *  (h)   6  DECLARATION ORDER `DrawFn d1; DrawFn d0;` -- later-declared takes the
 *      LOWER slot here, so this is what puts d0 at sp+0x8 and d1 at sp+0xc, the
 *      slots the loop's `ldr r4,[sp,#8]` / `ldr r4,[sp,#0xc]` name.
 *  (i)   4  The pinned r0/r1 block on the FIRST copy call (`q0`/`q1`), which is
 *      the only one of the three whose argument setup the ROM interleaves with
 *      `ldr r6,=Func_8001af8` and `adds r5,#128`.
 *  (j)   4  `DecompressLZ` declared to RETURN A VALUE -- makes gcc fill r0 LAST,
 *      giving the ROM's `mov r1,r8 / mov r0,r5`.
 *  (k)   4  `register void *tf __asm__("r0")` assigned as its own statement BEFORE
 *      `arg <<= 3`, so `ldr r0,=Task_BlitAnim` precedes `lsl r1,#3`.  The Anim_Vine
 *      sched2 lever, third function in this bank to need it.
 *
 * ================================================================
 * MEASURED INERT (all still 2 unless noted)
 * ================================================================
 *
 *   `DrawFn d[2]` as a real local array instead of two scalars ......... 2
 *   `DrawFn` declared `void` instead of `int` ......................... 2
 *   `StartTask` declared `int` instead of `void` ...................... 2
 *   `volatile` on the base+0x7780 store ............................... 2
 *   `volatile` on the base+0x7784 store ............................... 2
 *   volatile read of the gPtrs+0xbc slot .............................. 2
 *   `register int a1 __asm__("r1")` for the StartTask shift ........... 2
 *   all 4 statement orders x 3 StartTask shapes x 2 d-shapes (24 files)  2
 *   `-fno-strength-reduce` (diagnostic) ............................... 2
 *   `-fno-schedule-insns` i.e. sched1 (diagnostic) .................... 2
 *
 * CATASTROPHIC, and worth recording as a second confirmation of the prompt's
 * warning: `__asm__ volatile ("" : "+r" (d1))` after the d1 read gives 155
 * differing at count 287 -- it DELETES instructions.  The `"+r"` barrier is not
 * merely inert here, it is actively destructive.
 *
 * ================================================================
 * A MEASUREMENT-TOOL ARTEFACT, verified at the encoding level
 * ================================================================
 *
 * `scratch_elev/b292/E/norm.py` reports `ldrh r3, .L3` against the ROM's
 * `ldr r3, =0x100` as a difference.  IT IS COSMETIC.  gcc emits `ldrh rX, .L`
 * for a HImode constant store and gas assembles it to `4b01`, i.e.
 * `ldr r3,[pc,#4]` -- byte-identical to the ROM's form, since Thumb-1 has no
 * pc-relative `ldrh`.  Confirmed by objdump on both objects.  Both `0x100` and
 * `0xcc` go through the pool this way, so the two `REG_BG2PA` stores need no
 * shim at all.  Discount this pair whenever norm.py shows it.
 *
 * SHIMS THIS FILE USES, for fakematch.txt -- THREE, all pinned hard registers.
 * There are NO PIN MACROS, no `"+r"` barriers, no `volatile` beyond the io.h
 * register macros and the `volatile u16 *dst` in the Func_8001af8 / CopyFn
 * signature (which is the landed Anim_Break's own signature, not a shim of
 * ours), no DMA3_SET and no `.equ`:
 *   1. `register int q0 __asm__("r0")`   copy site 1   (with q1)
 *   2. `register void *q1 __asm__("r1")` copy site 1
 *   3. `register void *tf __asm__("r0")` StartTask
 *
 * HEADER-VS-BODY CORRECTION (checked at the encoding level, twice).  An earlier
 * draft of this header claimed FIVE pins and listed pins on copy sites 2 and 3.
 * Those two are GONE from the body: sites 2 and 3 now take separate `static
 * inline` bodies with the literal written inside each, which is batch 292's
 * separate-inline-bodies lever, and they are shim-free at the same 2 of 291.
 * `grep -c 'register .*__asm__'` on the body is 3, and the file still scores
 * `XX ENCODINGS differ in 2 place(s) (ref 291, ours 291)` with NO SIZE line and
 * NO relocation difference.  BOOK THREE ROWS, NOT FIVE.
 *
 * AND THE SUBSTITUTION IS PER SITE, WHICH IS A NEW LIMIT ON THAT LEVER.  Copy
 * site 1 does NOT accept an inline body -- it gives 4 of 291, because site 1 is
 * the one whose argument setup the ROM interleaves with `ldr r6,=Func_8001af8`
 * and `adds r5,#128`, and only the pinned r0/r1 pair orders those.  So an inline
 * body and a register pin are interchangeable on a PLAIN repeated-constant site
 * and NOT interchangeable where the site's argument setup is also interleaved.
 * Prefer the inline body (no shim); fall back to the pin where ordering is at
 * stake.
 *
 * No `.sym` proposal is warranted: the residue is a scheduler rank inside the
 * function, not a symbol boundary, and no symbol name would change it.
 * No per-file Makefile flag override applies -- no flag tested reaches 0, and a
 * flag row would need owner approval regardless.
  *
 * *** BATCH-305 CORRECTION: THIS FILE ATTRIBUTES A RESIDUE TO sched1, AND sched1 DOES NOT
 * *** RUN IN THIS BUILD.  Verified with -da at production flags: the dump sequence is
 * *** 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is NO sched1 dump,
 * *** because flag_schedule_insns is off at -O2 here, so only the post-reload scheduler runs.
 * *** Re-attribute to sched2 (rank_for_schedule), to combine, or to the ALLOCATION that fixed
 * *** the order.  Relatedly, any "-fno-schedule-insns is inert" note below rules nothing out:
 * *** that flag controls a pass that never runs.  The sched2 tie-break is priority ->
 * *** dependent count (more wins) -> INSN_LUID (lower wins), and LUID preserves EXPAND order.
 * *** See "sched1 DOES NOT RUN IN THIS BUILD" in docs/elevation.md.
*/
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

extern int *iwram_3001eec[];
extern void *gPtrs[];
extern u16 Lee096[] __asm__(".Lee096");
extern u8 Lee09c[] __asm__(".Lee09c");
extern u8 Lee09f[] __asm__(".Lee09f");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int DecompressLZ(void *src, void *dst);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int e);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

static inline void Copy0(CopyFn f, void *src)
{
    f((volatile u16 *)(0xa0 << 19), src, 0x80);
}

static inline void Copy1(CopyFn f, void *src)
{
    f((volatile u16 *)(0xa0 << 19), src, 0x80);
}

static inline void Copy2(CopyFn f, void *src)
{
    f((volatile u16 *)(0xa0 << 19), src, 0x80);
}

void BaseAnim_Spasm(void *context, int mode)
{
    vec3_t pos;
    void *ctx;
    DrawFn d1;
    DrawFn d0;
    int **tbl;
    int **pp;
    u8 *base;
    State **slot;
    CopyFn copy;
    u8 *data;
    int frame;
    int n;
    int x;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    data = GetFile(FILE_7b);
    {
        register int q0 __asm__("r0");
        register void *q1 __asm__("r1");
        q0 = 0xa0;
        q1 = data;
        copy = Func_8001af8;
        data += 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, q1, 0x80);
    }
    DecompressLZ(data, base);
    data = GetFile(FILE_8d);
    Copy1(copy, data);
    if (mode == 2) {
        data = GetFile(FILE_68);
        Copy2(copy, data);
    }
    GetBattleActorPos3((*slot)->ids[0], &pos);
    if (mode == 0) {
        REG_BG2PA = 0x100;
        x = 0x40 - pos.x;
        REG_BG2X = x << 8;
    } else {
        REG_BG2PA = 0xcc;
        x = -pos.x * 4 / 5 + 0x40;
        REG_BG2X = x << 8;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    d0 = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    d1 = (DrawFn)gPtrs[0xbc / 4];
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    if (mode == 2) {
        *(int *)(base + 0x77a8) = 0;
        _PlaySound(0xd4);
    } else if (mode == 1) {
        *(int *)(base + 0x77a8) = 8;
        _PlaySound(0xd4);
    } else {
        *(int *)(base + 0x77a8) = 0x20;
    }
    frame = 0;
    do {
        if (frame == 0) {
            if (mode == 2) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, -1, 0, 0x20);
            } else {
                Func_80d6888((*(State **)(base + 0x7828))->ids[0], 0xa, -1, 0, 0x20);
            }
        }
        if (frame == 0x18) {
            _Func_80bd7dc(0);
        }
        if (frame == 8 && mode == 0) {
            _PlaySound(0x7e);
        }
        if (frame < 0x20) {
            n = frame / 4;
            if (n > 2) {
                n = (n & 1) + 1;
            }
            if (frame < 0x1c) {
                d0(ctx, base + Lee096[n], 0x40 - Lee09c[n], pos.y - Lee09f[n] + 8,
                   Lee09c[n], Lee09f[n]);
                d1(ctx, base + Lee096[n], 0x40, pos.y - Lee09f[n] + 8,
                   Lee09c[n], Lee09f[n]);
            }
        }
        if (mode == 0) {
            UpdateScreenShake(2, 2);
        } else {
            UpdateScreenShake(0x10, 0x10);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x30);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
