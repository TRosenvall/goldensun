/* AnimStart2  [rom_c9000]  --  asm/rom_c9000/rom_cd508_a_c_a.s  @ 0x080cd86c
 *
 * NON-MATCHING: 83 encodings of 303 differ (objcmp).
 * Size 692 against the ROM's 696 (-4); 301 instructions against 303 (-2).
 * 83 is NOT a distance: the two-instruction deficit sits in the tile loop and
 * renames the registers of everything from the loop prologue to the epilogue.
 * All 17 relocations are present in the ROM's order with the same relocation
 * types, but FIVE OF THE SEVENTEEN STILL DIFFER, and they are the residue's own
 * fingerprint:
 *     gDMATaskCount's pool word at +4 (the first pool is one word out of place)
 *     both indirect calls `_call_via_r6` against the ROM's `_call_via_r5`
 *     WaitFrames and Func_80008d4 at -4 (the tile loop is two instructions short)
 * Everything before the tile loop is instruction-for-instruction correct apart
 * from the three local schedule swaps listed at the end.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/80cd86c.c \
 *     asm/rom_c9000/rom_cd508_a_c_a.s --func AnimStart2
 *
 * ================================================================
 * THIS RETIRES THE NAMED BLOCKER OF ITS SIBLING PARK (80cd594.c, AnimStart)
 * ================================================================
 *
 * AnimStart's park names its blocker as "the four queue-push constants are
 * CSE'd across the whole function": gcc builds 0x80<<19 once, parks it in r11
 * and copies it at each of four push sites, where the ROM rebuilds
 * `mov r2,#0x80 / lsl r2,#19` every time.  It records per-site named locals as
 * MEASURED INERT and lists "give each push its own copy of the inline body" as
 * NOT YET TRIED, calling it "same RTL, so unpromising".
 *
 * IT IS NOT UNPROMISING.  IT IS THE LEVER.
 *
 *   one `static inline QueuePush(u32 src, u32 dest)` called four times
 *                                         190 of 303 differ, 297 insns
 *   four `static inline QueuePush1..4(void)`, each with its own body and its
 *   own literals                           89 of 303 differ, 297 insns
 *
 * That single change:
 *   - rebuilds 0x80<<19 at all four sites (grep count 4, was 1) and 0x80<<10
 *     at all four sites (count 4, was 1) -- the ROM's shape exactly;
 *   - frees r11, which the ROM gives to `st`, so the prologue becomes the ROM's
 *     (`mov r11,r3` / `sub sp,#4` / one stack word for `arg`, against our
 *     previous two-word frame with both `st` and `arg` spilled);
 *   - puts base in r10, view in r9, mode in r7, REG_IME in r5,
 *     &gDMATaskCount in r6 -- all the ROM's registers.
 * The mechanism is NOT "the same RTL": with a parameter, the inliner lands the
 * actual argument as `(set (pseudo) (const_int K))` at the top of each inlined
 * block and cse2 (`-frerun-cse-after-loop`) then equates the four pseudos.  With
 * the literal written inside each body it expands directly into the store insn
 * at that site and cse2 leaves all four alone.  Confirmed by the flag: the one
 * -inline candidate rebuilds all four only under -fno-rerun-cse-after-loop.
 * THE CONSTANT-CSE PARAGRAPH IN 80cd594.c SHOULD BE STRUCK AND AnimStart
 * RE-OPENED WITH FOUR SEPARATE BODIES.
 *
 * ================================================================
 * SECOND LEVER: THE TILE LOOP'S `short` NARROWING NEEDS A VOLATILE STORE
 * ================================================================
 *
 * The ROM narrows the OR result through a `short` (`lsl r3,#16 / asr r3,#16`)
 * before the `strh`.  gcc drops the pair, because `strh` truncates anyway.
 * Seven store spellings were measured; only one keeps it:
 *
 *   `short v = a | b; *(unsigned short *)(vram + off) = v;`   89 of 303, 297
 *   `*(vu16 *)(vram + off) = (short)(a | b);`                 89 of 303, 297
 *   `int v = (short)(a | b); *(u16 *)(vram + off) = v;`       89 of 303, 297
 *   `short v = a | b; *(short *)(vram + off) = v;`            89 of 303, 297
 *   `short v = a | b; *(u16 *)&vram[off] = v;`                89 of 303, 297
 *   a file-scope `short v` instead of a block-scope one       89 of 303, 297
 *   `short v = a | b; *(vu16 *)(vram + off) = v;`   -->  88 of 303, 301 insns
 *
 * The volatile store is what forces the narrowing to be materialised: it is the
 * combination (a `short` LOCAL and a VOLATILE destination) that is load-bearing,
 * neither half alone.  That is +4 instructions and the ROM's
 * `lsl/asr/mov r9,r3/mov r3,r9` round trip.  It is in the candidate below.
 *

 * ================================================================
 * THIRD LEVER GROUP: THREE CALLEE/ARGUMENT SPELLINGS, WORTH 5 TOGETHER
 * ================================================================
 * Applied on top of the two levers above (88 -> 83).  Each was measured alone
 * and in combination; they are independent and all three are additive:
 *
 *   `_Func_80c0774` declared `void`, not `int`          88 -> 86
 *   `targ = 0xc8; targ <<= 4; StartTask(..., targ)`
 *       as its own statements, not `0xc8 << 4` inline   88 -> 86
 *   `Func_80cd508` declared `int`, not `void`           88 -> 87
 *   all three together                                  88 -> 83
 *
 * Note the int-return lever runs in OPPOSITE directions on the two callees in the
 * same function -- `_Func_80c0774` wants `void` and `Func_80cd508` wants `int` --
 * which is the per-callee direction rule from batch 291, confirmed again here with
 * both directions inside one translation unit.
 * The StartTask spelling is Anim_Curse's recorded lever RUNNING THE OTHER WAY:
 * there the ROM fills r0 BEFORE the shift and gcc fills it last; here the ROM
 * fills r0 AFTER the shift and gcc fills it first, and splitting the shift into
 * its own statements is what defers r0.
 * ================================================================
 * THE REMAINING BLOCKER: loop.c move_movables HOISTS ONE SHIFT TOO MANY
 * ================================================================
 *
 * Three shifted constants live in the tile loop.  The ROM hoists exactly two of
 * them and REBUILDS the third every outer iteration:
 *
 *   0x80 << 1   used at depth 1 (`a = row + (0x80<<1)`)   ROM: hoisted, r10
 *   0x80 << 2   used at depth 2 (`a += 0x80<<2`)          ROM: hoisted, r14
 *   0x80 << 5   used at depth 1 (`row += 0x80<<5`)        ROM: REBUILT inside
 *
 * We hoist all three (r10, r12, r8).  The third hoist steals r8, which the ROM
 * gives to the VRAM base, and that cascade is the entire residue:
 *   - vram lands in r14 instead of r8, so gcc can make a low copy
 *     (`mov r5,r14`) and use register-offset addressing `strh r3,[r5,r4]`,
 *     where the ROM must add (`mov r3,r8 / add r2,r5,r3 / strh r3,[r2]`) -- 1
 *     instruction short;
 *   - the outer-loop bottom is 4 instructions against the ROM's 9 -- 5 short;
 *   - the loop prologue is 14 against the ROM's 12 -- 2 long;
 *   - 0x80<<7 is hoisted into r5 across the two `clear()` calls where the ROM
 *     rebuilds it, which is why our calls go through `_call_via_r6` and the
 *     ROM's through `_call_via_r5` -- 1 short.
 *
 * The pass is loop.c's move_movables (the lifetime test at loop.c:1803).  The
 * AnimStart park asserts this asymmetry "comes out right from the plain shift
 * spellings -- do not reach for locals"; that is true in AnimStart and FALSE
 * here, and the difference is that AnimStart's post-loop tail keeps three more
 * values live (its base+0x77a8 / +0x77a0 / +0x77a4 stores), which AnimStart2
 * does not have.  The asymmetry is therefore a register-pressure readout, not a
 * property of the spelling.
 *
 * MEASURED INERT (all at 87-88 of 303, 301 instructions, i.e. no change):
 *   - `row = row + (0x80<<5)`, `row += 0x1000` (same constant, same fold);
 *   - `__asm__ volatile ("" : "+r" (row))` after the increment -- 92 of 303,
 *     305 insns, WORSE;
 *   - a `"+r"` volatile barrier on a named length local at the first clear call
 *     (87 of 303, 301) and at the second (87 of 303, 301) -- it does NOT restore
 *     the rebuild;
 *   - outer loop as a goto loop (90 of 303, 299); both loops as goto loops
 *     (89, 297); inner as a goto loop (88, 301, identical to the do-while).
 *   - ANIM_BREAK'S giv LEVER ("per-target induction variables belong as
 *     strength-reduced givs", src/rom_c9000/rom_d82b0_b.c) DOES NOT REACH THIS
 *     LOOP.  `a = n * (0x80<<5) + (0x80<<1)` and `b = n * 0x10` with `row`/`col`
 *     deleted screens at 69 of 303 -- a SMALLER NUMBER THAT IS NOT A DISTANCE:
 *     672 bytes against 696 and 291 instructions against 303, a 12-instruction
 *     deficit, because strength_reduce rebuilds both accumulators and drops the
 *     outer-loop bottom entirely.  `row` as a giv with `col` left an accumulator
 *     is 146 of 303 / 294 insns; `row` pre-biased to 0x80<<1 with the add moved
 *     out of the loop body is 84 of 303 / 299 insns.  All three are worse than
 *     83 once length is counted, and they are recorded here so the giv lever is
 *     not re-attempted on this loop.
 *   - THE TILE-LOOP STORE ADDRESS, four spellings, AND ONE OF THEM OVERSHOOTS.
 *     `*(vu16 *)(0x6003800 + off) = v` with no `vram` local is 72 of 303 at 305
 *     instructions and 700 bytes -- TWO INSTRUCTIONS TOO MANY, the opposite sign
 *     from this park's -2, so the correct spelling is bracketed by these two and
 *     is neither.  Adding the outer goto loop on top of it lands on 71 of 303 at
 *     299 (-4).  A plain pointer walk (`*(vu16 *)vram = v; vram += 2;`) is 68 of
 *     303 at 297 (-6) -- a smaller number that is NOT a distance.
 *     `*(vu16 *)(off + (int)vram)` is 83 of 303 at 301, identical to the file
 *     below (the operand-order lever is inert on a CONSTANT-offset address, which
 *     is consistent with Anim_Curse's finding that it is conditional on the
 *     offset being non-constant).
 *   - flags: -fno-gcse (88/301, identical), -fno-strength-reduce,
 *     -fno-cse-follow-jumps, -fno-thread-jumps, -fno-caller-saves,
 *     -fno-force-mem (all 88/301, identical);
 *     -fno-rerun-cse-after-loop 285/311 WORSE, -fno-schedule-insns2 158/299
 *     WORSE, -fno-expensive-optimizations 196/299 WORSE.
 *
 * ================================================================
 * THE `"+r"` BARRIER DOES NOT DEFEAT cse2 ON A REPEATED POOL CONSTANT HERE
 * ================================================================
 *
 * Batch 291 recorded `__asm__ ("" : "+r" (v))` as a per-site alternative to a
 * -fno-gcse / -fno-rerun-cse-after-loop file flag, to be placed on the FIRST use
 * of a repeated constant.  On this function it is inert in every placement:
 *
 *   non-volatile `__asm__ ("" : "+r" (dest))` at the top of the single inline
 *                                          197 of 303, 297 insns (was 190)
 *   `__asm__ volatile ("" : "+r" (dest))`  202 of 303, 295 insns
 *
 * and in both cases the constant is STILL built once and copied -- it merely
 * migrates from r11 to r8.  The reason is structural: `"+r"` expands to a copy
 * in and a copy out, so the pre-asm pseudo still carries the known constant and
 * cse2 reuses THAT at the later sites.  A barrier can make a value opaque
 * downstream of itself; it cannot un-know a constant upstream of itself.  The
 * batch-291 note should be qualified with that limit.  What works instead is
 * removing the shared pseudo altogether -- four separate inline bodies, above.
 *
 * ================================================================
 * REMAINING NON-LOOP DIFFERENCES (all schedule swaps, no length effect)
 * ================================================================
 *   - prologue: ours hoists `ldr r5,[r3,#4]` (arg) two insns early and sinks
 *     `mov r9,r2` (view) past `mov r7,r0`;
 *   - `mov r3,#1 / mov r2,r11` comes out in the opposite order to the ROM's;
 *   - REG_BLDCNT=0: the ROM loads the address into r2 and REG_IME into r5, ours
 *     reuses r5 for both in sequence;
 *   - `_Func_80c0774`'s two immediate arguments (`#1`, `#0x80`) and StartTask's
 *     `lsl r1,#4 / ldr r0,=Func_80cd4b4` come out in the opposite order.
 *   Batch 291's "at a two-register-argument call try the callee's return type
 *   before argument precompute" was NOT tried on these four sites; that is the
 *   cheapest next experiment after the move_movables question.
 *
 * ================================================================
 * SPLIT SHAPE
 * ================================================================
 * asm/rom_c9000/rom_cd508_a_c_a.s holds THREE functions
 * (`grep -ci func_start` = 3): Func_80cd52c @ 0x080cd52c, AnimStart @ 0x080cd594,
 * AnimStart2 @ 0x080cd86c, and NO data section.  AnimStart2 is the LAST function,
 * so `tools/split_s.py asm/rom_c9000/rom_cd508_a_c_a.s AnimStart2` cuts cleanly.
 * NO LABEL CROSSES THE BOUNDARY IN EITHER DIRECTION: every `.L*` in AnimStart2's
 * range is an in-body branch target or a mid-function literal pool that gcc
 * regenerates, and nothing outside the function references one.  ZERO `.global`
 * exports are required.  (datacheck's EXPORTS line is not the set a split needs;
 * it lists what is already `.global`.)
 *
 * No new symbol is warranted.
 */
#include "gba/types.h"
#include "gba/io.h"

typedef void (*ClearFn)(void *dst, s32 len);

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern int *iwram_3001eec[];
extern unsigned short iwram_3001ad0[];

extern int Func_80cd508(void);
extern void WaitFrames(unsigned int n);
extern void _Func_80c0774(int a, unsigned short b, int c);
extern void StartTask(void *fn, int arg);
extern void Func_80cd4b4(void);
extern int _Func_80c0cec(int a, int b, int c, int d);
extern void Func_80008d4(void *dst, s32 len);

static inline void QueuePush1(void)
{
    struct DmaQueue *queue;
    u32 savedIme;
    int count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = queue->count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0x1741;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePush2(void)
{
    struct DmaQueue *queue;
    u32 savedIme;
    int count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = queue->count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0x1341;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePush3(void)
{
    struct DmaQueue *queue;
    u32 savedIme;
    int count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = queue->count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0x1341;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePush4(void)
{
    struct DmaQueue *queue;
    u32 savedIme;
    int count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = queue->count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0x7741;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}


void AnimStart2(int mode)
{
    unsigned char *base;
    unsigned char *view;
    void *arg;
    int *st;
    unsigned char *vram;
    ClearFn clear;
    int off;
    int row;
    int col;
    int n;
    int m;
    int a;
    int b;
    int targ;

    base = (unsigned char *)iwram_3001eec[0];
    view = *(unsigned char **)((char *)iwram_3001eec - 0x78);
    arg = (void *)iwram_3001eec[1];
    st = (int *)iwram_3001eec[5];
    Func_80cd508();
    st[3] = 1;
    WaitFrames(1);
    REG_BLDCNT = 0;
    QueuePush1();
    iwram_3001ad0[3] = 0x20;
    WaitFrames(1);
    _Func_80c0774(1, *(unsigned short *)(view + (0xc9 << 3)), 0x80);
    *(int *)(base + 0x77b4) = 0x18;
    *(int *)(base + 0x77b8) = 0;
    targ = 0xc8;
    targ <<= 4;
    StartTask(Func_80cd4b4, targ);
    QueuePush2();
    WaitFrames(1);
    REG_BG2CNT = mode | 0x784;
    QueuePush3();
    _Func_80c0cec(0, 0, 0, 0x64);
    st[3] = 0;
    WaitFrames(1);
    REG_BLDCNT = 0x3f44;
    REG_BLDALPHA = 0x100e;
    REG_BG2X = 0;
    REG_BG2Y = 0xfffff000;
    REG_BG2PA = 0x80;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 0x100;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;
    QueuePush4();
    WaitFrames(1);
    off = 0;
    vram = (unsigned char *)0x6003800;
    n = 0;
    col = 0;
    row = 0;
    do {
        a = row + (0x80 << 1);
        m = 0;
        b = col << 1;
        do {
            short v = a | b;
            *(vu16 *)(vram + off) = v;
            a += 0x80 << 2;
            b += 2;
            off += 2;
            m++;
        } while (m != 8);
        row += 0x80 << 5;
        n++;
        col += 8;
    } while (n != 0x10);
    clear = Func_80008d4;
    clear(arg, 0x80 << 7);
    clear((void *)0x6004000, 0x80 << 7);
    WaitFrames(1);
}
