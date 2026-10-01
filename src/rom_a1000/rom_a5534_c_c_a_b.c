/* Func_80a65e4 @ 0x080a65e4  --  BYTE-IDENTICAL, BUT WITH ONE FAKEMATCH-CLASS SHIM
 *
 * FIGURE: 0, with a shim.  Measured, not inherited:
 *     OK Func_80a65e4 -- 48 bytes, 21 encodings and 1 relocations identical
 *     OK whole file   -- 48 bytes, 21 encodings and 1 relocations identical
 *     tools/shimcount.py: "+r" barriers : 1  (classed as a fakematch)
 *                         *** has a fakematch-class shim and NO fakematch.txt row
 * DEVICE-FREE BEST: 7 of 21, which is the body below with the one
 * `__asm__("" : "+r" (g))` deleted.  Baseline for the park body in
 * src/non_matching/rom_a1000/80a65e4.c was 10 of 21.
 *
 * SO THIS IS A DECISION, NOT A LANDING.  Install only if a fakematch.txt row is
 * acceptable here; otherwise take the 10 -> 7 improvement and the diagnosis.
 *
 * Verify with:
 *     docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/rom_a1000/rom_a5534_c_c_a_b.c \
 *       asm/rom_a1000/rom_a5534_c_c_a_b.s --whole
 *
 * SPLIT.  asm/rom_a1000/rom_a5534_c_c_a.s holds FOUR functions.
 *   python3 tools/datacheck.py asm/rom_a1000/rom_a5534_c_c_a.s -> CLEAN (no data)
 *   python3 tools/split_s.py asm/rom_a1000/rom_a5534_c_c_a.s Func_80a65e4 --dry-run
 *     would write ..._c_a_a.s  (1 function, 244 lines) [Func_80a63e4]
 *     would write ..._c_a_b.s  (1 function, 31 lines)  [Func_80a65e4]
 *     would write ..._c_a_c.s  (2 functions, 286 lines)[Func_80a6614, Func_80a6794]
 *     would REMOVE ..._c_a.s ; would rewrite stage1.ld
 * INSTALL AT: src/rom_a1000/rom_a5534_c_c_a_b.c, deleting
 *             asm/rom_a1000/rom_a5534_c_c_a_b.s after the split.
 * AFTER LANDING, repoint three sibling recipes that all name the dead stem:
 *   src/non_matching/rom_a1000/80a63e4.c  -> asm/rom_a1000/rom_a5534_c_c_a_a.s
 *   src/non_matching/rom_a1000/80a6614.c  -> asm/rom_a1000/rom_a5534_c_c_a_c.s
 *   src/non_matching/rom_a1000/80a6794.c  -> asm/rom_a1000/rom_a5534_c_c_a_c.s
 *
 * THE PARK'S DIAGNOSIS IS CORRECT AND ITS CONCLUSION IS WRONG.
 * It says: "the address add is folded into the store's addressing mode ... the
 * sum is only needed by the store, so gcc sinks it into the addressing mode no
 * matter what the source calls it.  That is a real limit on the named-pointer
 * lever."  The first half is right -- and I confirm its measurement: naming the
 * sum is byte-identical (10).  The second half is wrong: the fold is not reached
 * by NAMING the sum, it is reached by QUALIFYING THE STORE.
 *
 * MECHANISM, three crossed parts.  The park tried part 2 alone.
 *
 *   1. `g += k;` -- the add written IN PLACE, so its destination is the base.
 *      The ROM's `add r3, r2` is the two-operand *thumb_addsi3 form, which
 *      requires rd == operand 1; a fresh name `p = g + k` can only give the
 *      three-operand `add r3, r2, r3`.  ALONE: 10 (exactly inert, because the
 *      add is still folded away).
 *   2. a named sum / `p = g + k`.  ALONE: 10 (the park's own result, confirmed).
 *   3. `*(volatile short *)g = v;` -- combine will not fold an address into a
 *      VOLATILE MEM, so the add survives as a real insn.  ALONE, folded into the
 *      park's shape: 10 (exactly inert).
 *   1 + 3: 7  -- 20 instructions against 20, with `add r3,r2 / strh r0,[r3,#0]`
 *                exactly right.  Residue is only the g/k register roles.
 *   2 + 3: 6  -- store register right, add operand order wrong.
 *   1 + 3 + the barrier below: 0.
 *
 * WHAT THE REMAINING 7 ACTUALLY IS, taken off the dumps.
 *  (a) THE g/k REGISTER SWAP.  .17.lreg gives, for the 1+3 body:
 *         g   used 5 times across 32 insns  ->  floor_log2(5)*5/32 = 0.3125
 *         k   used 3 times across  8 insns  ->  floor_log2(3)*3/8  = 0.375
 *      so k is allocated first and takes r3 (REG_ALLOC_ORDER reaches r3 before
 *      r2 here), leaving g in r2.  The ROM has it the other way.  Nothing
 *      source-level moves it: every arrangement of the declarations, of the two
 *      assignments inside each arm, and of the m/v block was measured (see the
 *      inert list) and the only ones that changed anything changed it for the
 *      worse by collapsing the if/else into a conditional skip.
 *  (b) ONE TRANSPOSITION IN ARM 1.  -fsched-verbose=6 on the arm's block:
 *         insn 31  ldr r3,=gState   prio 1  cost 2  dependents: {34}      = 1
 *         insn 90  mov r2,#0x88     prio 2  cost 1  dependents: {34, 91}  = 2
 *         insn 91  lsl r2,#2        prio 1
 *      The pick is made on PRIORITY, not on a tiebreak: the mov is one step
 *      further from the end of the block than the independent pool load, so
 *      sched2 must take it first.  For the ROM's order the pool load would need
 *      an in-block TRUE dependent, i.e. the add would have to live inside the
 *      arm -- and it cannot, there is only one add and two arms.  This is a
 *      dump-backed statement that the arm's shape alone cannot produce the ROM's
 *      order, which is why the only thing that reaches it is a barrier.
 *
 * The barrier in arm 1 closes BOTH (a) and (b) at once -- it is the single
 * fakematch-class construct in the file, and dropping it returns exactly 7.
 *
 * MEASURED INERT OR WORSE (all against the park's 10 unless stated):
 *   naming the sum (the park's own lever)                                  10
 *   `g = g + k` as a fresh statement                                       10
 *   `k += (unsigned)g` (offset as the add's destination)                   10
 *   g as `unsigned int` rather than a pointer, in place                    10
 *   g as `short *` with the cast on the add                                10
 *   volatile store without the in-place add                                10
 *   base hoisted above the if (arms then set only k)       17, RELOCDIFF, -4 bytes
 *   both arms building a whole pointer                     16, RELOCDIFF, -8 bytes
 *   `k + (int)&gState`                                     18, -4 bytes
 *   extern short array indexed in bytes                    18, RELOCDIFF
 *   gState itself declared volatile                        7  (no better than 1+3)
 *   k's two values as `k = 0x88; k <<= 2;`                 7  (no better)
 *   arms writing k before g                                14, RELOCDIFF
 *   declaring k before g                                   7  (exactly inert)
 *   m/v block moved below the if                           22, +4 bytes
 *   k declared volatile                                    23, +12 bytes
 *   register pin g->r3 (no barrier)                        2
 *   register pin k->r2 (no barrier)                        2
 *   `__asm__("" : "+r" (g))` after the add (no pin)        2
 *   barrier in BOTH arms                                   2
 *
 * The bit-packing at the top is the park's and it is right: `m = 0x3fff; m &= b;`
 * makes the constant the AND's destination, and writing the `c == 0` arm first
 * makes it the fall-through, as in the ROM.
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;

int Func_80a65e4(int a, int b, int c)
{
    unsigned char *g;
    unsigned int k;
    int v;
    int m;

    m = 0x3fff;
    v = a << 10;
    m &= b;
    v |= m;
    if (c == 0) {
        g = (unsigned char *)&gState;
        __asm__("" : "+r" (g));
        k = 0x88 << 2;
    } else {
        g = (unsigned char *)&gState;
        k = 0x222;
    }
    g += k;
    *(volatile short *)g = v;
    return 1;
}
