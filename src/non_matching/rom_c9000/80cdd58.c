/* InitRenderTilemapBG1 -- asm/rom_c9000/rom_cd508_c_a.s, 0x080cdd58, 107 ROM lines.
 *
 * NON-MATCHING: 2 encodings of 129 differ (objcmp).
 *
 * SIZE EXACT (312 bytes = 312), INSTRUCTION COUNT EXACT (129 = 129),
 * RELOCATIONS EXACT (objcmp prints no RELOCATIONS line).  So 2 IS a true
 * distance.  rom_cd508_c.s has NO data section (tools/datacheck.py prints
 * nothing for it); the split is text-only, and Anim_PlanetDiver / Anim_Haunt
 * stay in assembly beside DrawLine and Anim_Confuse.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/cdd58_InitRenderTilemapBG1.c \
 *     asm/rom_c9000/rom_cd508_c_a.s --func InitRenderTilemapBG1
 *
 * THE RESIDUE, exactly: ONE sched2 SWAP IN THE LOOP PREHEADER.
 *   rom  ... mov r0,#0 / mov r6,#0 / mov r12,r2 / mov r4,#0
 *   ours ... mov r0,#0 / mov r6,#0 / mov r4,#0  / mov ip,r2
 * `mov r12,r2` is reload's copy for the 0x100 loop invariant (thumb cannot
 * shift a hi register, so the value is built in r2 and copied to ip).  loop.c
 * inserts a hoisted invariant with emit_insn_before (loop_start), i.e. AFTER
 * everything expand already put in the preheader -- and `row = 0` comes from
 * the `for` init, which expand emitted there.  So gcc's pre-sched2 order has
 * `row = 0` first, both insns are ready together in the preheader block, their
 * sched2 priorities tie at 0 (both die across the block boundary), and
 * rank_for_schedule falls through to INSN_LUID.  PASS: sched2 (haifa-sched.c
 * rank_for_schedule, LUID tie-break), forced by loop.c's invariant insertion
 * point.  No source order reaches it: `row = 0` is already the last
 * initialisation the source can express (`for (i = 0, row = 0; ...)` beats
 * `row = 0;` before the loop by 4 encodings; putting `o = 0` in the for-init
 * too costs 2).  Giving 0x100 its own source local DOES put its materialisation
 * first -- and costs 98 of 129 AND TWO INSTRUCTIONS, because a source local
 * gets a LOW register and the `mov ip,r2` / `mov r3,ip` pair disappears.  The
 * hi-register 0x100 only exists because it is a compiler pseudo under pressure.
 *
 * ============================================================
 * TWO HARD-REGISTER PINS ARE LOAD-BEARING HERE.  Without them this file is
 * 18 of 129 (same size, same count).  Both are the Tackle-park device
 * ("give a compiler-generated operand a source statement by pinning it to the
 * register the ROM uses"), and the r1 one is this bank's own recorded
 * `invalidate_for_call` lever.  They are reported, not smuggled:
 *
 *  (1) `register int n __asm__("r1")` for the size argument of the last two
 *      calls -- worth 8.  MECHANISM, PROVEN: the ROM re-materialises
 *      `mov r1,#0x80 / lsl r1,#7` at BOTH calls; gcc shares one pseudo.
 *      expand makes a pseudo per constant argument (.00.rtl insns 245 and 255,
 *      `(set (reg 87) (const_int 16384))` / `(set (reg 89) (const_int 16384))`),
 *      cse1 unifies 89 into 87 (COST of a CONST_INT is 0 under ARM's
 *      CONST_COSTS because const_ok_for_arm(0x4000) holds, so a REG at cost 0
 *      ties and wins), local_alloc then gives 87 a CALLEE-SAVED register
 *      because it crosses the call -- and that steals r5 from the function
 *      pointer, which is why the ROM's `bl _call_via_r5` reads
 *      `bl _call_via_r6` without the pin.  A hard call-clobbered register is
 *      invalidated by cse's invalidate_for_call, so the pin breaks the
 *      unification.  CONTROL: giving the second call a DIFFERENT constant
 *      (0x80 << 8) reproduces the ROM's whole tail with no pin -- the tail
 *      collapses to the `lsl` alone -- which isolates the sharing as the only
 *      cause.  Eight source spellings of the same value (named local, two
 *      locals, re-assigned pointer, `unsigned`, K&R pointer type, casts,
 *      0x100 << 6) are ALL INERT at 20/18.
 *  (2) `register int row __asm__("r4")` -- worth 5, and the PASS is
 *      global_alloc.  `.18.greg` prints the priority order as
 *      `43 41 42 60 45 40 62 58 37` = t, j, o, BASE, row, i; the ROM needs
 *      row before base (base is r5, row r4, i r6).  base is pseudo 60, the
 *      0x600fb00 pool value loop.c hoists; row is pseudo 45.  Both carry 7
 *      loop-depth-weighted refs, so floor_log2(n_refs)*n_refs is 14 for each
 *      and the order is decided by live_length: base dies at the last inner
 *      store, row at the outer `row += 0x10`, so base's range is the shorter
 *      and it wins.  Closing that needs ONE more weighted reference on row
 *      (7 -> 8 flips floor_log2 to 3 and the numerator to 24) -- i.e. an
 *      instruction the ROM does not have.  BY THE .17.lreg RULE THIS ONE IS
 *      UNREACHABLE BY ARITHMETIC from any spelling, which is why the pin is
 *      here.  Declaration order is INERT (four permutations, all 20/18),
 *      consistent with every local being register-resident.
 *
 * ============================================================
 * SIX LEVERS THAT LANDED HERE, 127 -> 2.  All measured, drop ladder run.
 *
 *  (a) A (reg + LARGE CONSTANT) HImode ADDRESS BECOMES `add rD,rN,rM / strh
 *      [rD]`, BUT (reg + reg) FROM TWO VARIABLES BECOMES `strh [rB,rO]`.
 *      Writing the VRAM base as the literal `0x600fb00` inside the address
 *      (`*(u16 *)(o + 0x600fb00)`) is what produces the ROM's two separate
 *      `add r3,r0,r5`; a named `vram` pointer gives `strh r6,[r1,r5]` and
 *      loses 2 instructions.  126 of 129 with the wrong SIZE -> 45 of 129
 *      with the size and count EXACT, in one edit.  Same shape as the landed
 *      src/rom_a1000/rom_a5534_a_b.c (`ldr r2,=914 / add r3,r5,r2 / strh`).
 *  (b) `volatile` ON THE VRAM STORE STOPS loop.c BUILDING A WALKING POINTER.
 *      36 -> 26.  Without it strength reduction makes the inner address a giv,
 *      hoists `add r3,r0,r5` above the `if` and walks it with `add r3,#2`;
 *      the ROM recomputes it in BOTH arms.  This is the cheap alternative to
 *      writing the inner loop as a goto loop -- which also removes it from
 *      loop.c's reach but costs the outer loop its invariant hoisting
 *      (292 bytes, 119 instructions).
 *  (c) THE RETURN TYPE OF AN INDIRECT-CALL POINTER IS PER CALL SITE, NOT PER
 *      CALLEE.  `int (*)(void *, int)` for the two early calls is worth 9 and
 *      fixes the pool word order (it makes gcc load the function pointer
 *      BEFORE r0, which is the ROM's order); `void (*)(void *, int)` for the
 *      two tail calls is worth a further 2.  The SAME callee, Func_80008d4, is
 *      reached through an `int` pointer at one site and a `void` pointer at
 *      another IN ONE FUNCTION.  `int` on the tail pointer costs 2, `void` on
 *      the early one costs 9, and `void` on fq costs 7.
 *  (d) A NAMED LOCAL POINTER DEFEATS THE symbol+offset POOL FOLD.
 *      `*(u16 *)(iwram_3001ad0 + 6)` pools `iwram_3001ad0+6`; `p =
 *      iwram_3001ad0; *(u16 *)(p + 6)` pools the plain symbol and keeps the
 *      offset in `strh [r2,#6]`, which is the ROM.  Same for iwram_3001e74 --
 *      and that ALSO fixed the prologue: with the three loads reached off one
 *      named pointer gcc emits `ldr r3,=sym / ldr [r3,#0x7c] / ldr [r3] /
 *      add r3,#0x8c / ldr [r3]`, byte-for-byte the ROM, where the folded form
 *      picked `sym+124` as the pool word and cost two instructions.
 *  (e) AN `int` CARRIER FOR A HALFWORD CONSTANT STORE.  `z = 0x20;
 *      *(u16 *)(p + 6) = z;` gives `mov r3,#0x20`; the bare literal gives a
 *      POOLED `ldr r3,=32`.  Confirmed in isolation.  But the SAME function's
 *      ten REG_* stores all want the BARE literal (the ROM pools 0x7741,
 *      0x1f81, 0x3f42, 0xf0, 0x1088, 0x3537, 0x3f21, 0x100e -- including 0xf0
 *      and 0xff, both of which FIT `mov #imm8`), so the carrier question is
 *      per store, not per function.  Eleven halfword constants, ONE carrier.
 *  (f) `zr = 0;` WRITTEN BEFORE `z = 0x20;` -- 26 -> 20.  The zero then lands
 *      in r1 before the 0x20 reaches r3, which is the ROM's
 *      `mov r1,#0 / mov r3,#0x20 / strh / str r1`.  Reordering the two STORES
 *      instead is inert (26).  And `j = 0;` written before `t = row + 0x100;`
 *      inside the outer body is worth 6 -> 4 on top of everything else: both
 *      are batch 290's "i = 0 written early", twice in one function.
 *
 * MEASURED NEGATIVES (do not retry):
 *  - `-fno-gcse`, `-fno-rerun-cse-after-loop`, `-fno-schedule-insns2`,
 *    `-fno-strength-reduce`, `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`,
 *    `-fno-expensive-optimizations`, `-fno-caller-saves`, `-fno-force-mem`:
 *    ALL leave the residue at 19.  No per-file Makefile row is warranted.
 *  - Declaration order: 4 permutations of `row` against `base` and `i`, all
 *    inert.  SLOT lever only; every local here is register-resident.
 *  - `t = i * 0x10 + 0x100` / `(i << 4) + 0x100` / `(i + 0x10) << 4` instead of
 *    an explicit `row`: 98 of 129 and 127 instructions.  gcc does NOT
 *    strength-reduce it here; the explicit local IS the ROM's r4.
 *  - `row += 0x10` moved above the inner loop: 51.  `unsigned row`: inert.
 *  - REG_BLDCNT (0x4000050) is reached as `ldr =REG_BG1CNT (0x400000a) /
 *    add r2,#0x46` and REG_BLDALPHA (0x4000052) as
 *    `ldr =REG_WININ (0x4000048) / add r2,#2 / add r2,#8`.  Both come out of
 *    plain `REG_BLDCNT = ...` / `REG_BLDALPHA = ...` from include/gba/io.h --
 *    the move2add chains are automatic, no spelling needed.
 */
#include "gba/io.h"

extern unsigned char iwram_3001e74[];
extern unsigned char iwram_3001ad0[];
extern void Func_80cd508(void);
extern void _Func_80c0774(int a, unsigned short b, int c);
extern void Func_80008d4(void *dst, int n);
extern void Func_80008d8(void *dst, int n, int v);
extern void WaitFrames(unsigned int n);

void InitRenderTilemapBG1(void)
{
    int (*fp)(void *, int);
    int (*fq)(void *, int, int);
    void (*fr)(void *, int);
    unsigned char *st;
    unsigned char *ad0;
    unsigned char *dst;
    unsigned char *tbl;
    unsigned char *base;
    int i, j, o, t, z, zr;
    register int row __asm__("r4");

    st = iwram_3001e74;
    tbl = *(unsigned char **)st;
    dst = *(unsigned char **)(st + 0x7c);
    base = *(unsigned char **)(st + 0x8c);
    Func_80cd508();
    _Func_80c0774(2, *(unsigned short *)(tbl + (0xc9 << 3)), 0);
    ad0 = iwram_3001ad0;
    zr = 0;
    z = 0x20;
    *(unsigned short *)(ad0 + 6) = z;
    *(int *)(base + 0xc) = zr;
    fp = Func_80008d4;
    fp((void *)0x6003fc0, 0x40);
    fq = Func_80008d8;
    fq((void *)0x600f900, 0x80 << 2, -1);
    o = 0;
    for (i = 0, row = 0; i != 0x10; i++) {
        j = 0;
        t = row + 0x100;
        for (; j != 0x20; j++) {
            if (j > 0xf)
                *(volatile unsigned short *)(o + 0x600fb00) = 0xff;
            else
                *(volatile unsigned short *)(o + 0x600fb00) = t;
            t++;
            o += 2;
        }
        row += 0x10;
    }
    REG_DISPCNT = 0x7741;
    REG_BG1CNT = 0x1f81;
    REG_BLDCNT = 0x3f42;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;
    REG_BLDALPHA = 0x100e;
    fr = Func_80008d4;
    {
        register int n __asm__("r1");
        n = 0x80 << 7;
        fr(dst, n);
        n = 0x80 << 7;
        fr((void *)0x6004000, n);
    }
    WaitFrames(1);
}
