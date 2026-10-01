/* OvlFunc_971_2008128 -- asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s, 9 instructions.
 * NON-MATCHING, 2 of 9 encodings (re-measured batch 317 as installed).
 * Park: src/non_matching/ovl_7fb4a8/2008128.c  (ALSO class-parked in
 * src/non_matching/tiny_reg_order.c)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s --func OvlFunc_971_2008128
 *   (while parked, point the first argument at src/non_matching/ovl_7fb4a8/2008128.c)
 *
 * FIGURE IN  : 7 of 9  (measured, not inherited -- 13 encodings against 13, SIZE
 *                       and RELOCATIONS exact; --whole agrees at 7 of 13)
 * FIGURE OUT : 2 of 9
 *
 * NO SPLIT NEEDED: datacheck.py reports no data section and the .s holds this
 * one function only (it was split out in an earlier batch).
 * PINS: 0.
 *
 * TWO PARK CLAIMS REFUTED, ONE FIGURE CORRECTED
 * ---------------------------------------------
 * 1. tiny_reg_order.c records this as "6 of 9". It is SEVEN. Nothing in the
 *    tree verified it; there was no --func recipe naming this function.
 * 2. Both parks diagnose the blocker as OPERAND ORDER -- tiny_reg_order.c:
 *    "the ROM's store is `str r2, [r3, r4]` with the SCALED INDEX as the base
 *    and the array address as the offset -- the reverse of what the
 *    pointer-typed-operand lever produces. Writing the store through explicit
 *    char* arithmetic in either direction does not swap them." THAT IS SOLVED
 *    AND IT WAS NEVER THE BLOCKER. Both register-offset accesses come out in
 *    the ROM's shape, in integer space, exactly as the brief's PLUS-operand-
 *    order lever predicts: `*(int *)(off + cbase)` and `*(int *)(k + ebase)`
 *    with off/k/cbase/ebase all `int`. The park's own body already had that
 *    much; what it lacked was the register assignment, below.
 * 3. "The three pool loads are emitted in a different order from the ROM's,
 *    which is where the seven come from" -- REFUTED. The pool loads are in the
 *    ROM's order in the candidate below, and all five register roles match.
 *
 * WHAT ACTUALLY MOVED IT: 7 -> 5 -> 2, two levers CROSSED
 * -------------------------------------------------------
 * The park's body reaches both symbol bases as inline symbols inside the MEMs,
 * so RELOAD materialises them and reload REUSES ONE REGISTER for two unrelated
 * bases. Read off -fsched-verbose=6 on the park's body: insn 31 (`r3 = CHAR
 * pool`) carries dep 2 -- an output dependence on the earlier `r3 = L1940 pool`
 * and an anti-dependence on the ldrb that reads r3 -- so it CANNOT issue early,
 * and the ldrb takes the slot the ROM gives to `lsl r1, r0, #2`.
 *
 *   LEVER 1: NAME THE BASE POINTERS AS LOCALS so each becomes an allocno
 *            instead of a reload. `t = L1940` alone takes 7 -> 5 and puts the
 *            ldrb in the ROM's position; the same false dependence then
 *            reappears one level down on r2 (`i<<2` and the ewram base sharing
 *            it, insn 34 dep 2).
 *   LEVER 2: NAME ALL THREE (t, cbase, ebase) so the function has the ROM's
 *            FIVE distinct quantities -- i(r0), off(r1), cbase->v(r2),
 *            t->k->k<<2(r3), ebase(r4). Crossed with lever 1 this takes it to
 *            2 of 9 and every one of the eight register assignments is then
 *            the ROM's.
 *
 * The statement ORDER among those five assignments matters and was swept
 * exhaustively: all 120 legal permutations (scratch_elev/b317/A/w4/, generator
 * gen2.py, results RESULTS.txt). Histogram: 17 variants at 2, 11 at 4, 35 at 5,
 * 20 at 6, 37 at 7. SEVENTEEN different orders reach 2 and all seventeen emit
 * the IDENTICAL stream, which is itself the finding -- see below.
 *
 * THE RESIDUE, AND WHY NO SOURCE SPELLING REACHES IT
 * --------------------------------------------------
 * 2 of 9 is ONE ADJACENT SWAP of two INDEPENDENT instructions:
 *
 *     rom   ... ldr r2,=CHAR | lsl r1,r0,#2    | ldrb r3,[r3,r0] | ldr r4,=ewram ...
 *     ours  ... ldr r2,=CHAR | ldrb r3,[r3,r0] | lsl r1,r0,#2    | ldr r4,=ewram ...
 *
 * Indices 0,1 and 4,5,6,7,8 are exact. The two swapped insns write different
 * registers (r1, r3) and both merely read r0.
 *
 * Read off the ranker at the deciding cycle (t=4, last_scheduled_insn = the
 * CHAR pool load), for the best variant p_EOCTKV:
 *
 *     insn 17  lsl r1 = r0<<2      prio 36   dependents: 49 48 31          = 3
 *     insn 28  ldrb r3 = [r3+r0]   prio 36   dependents: 49 48 37 34       = 4
 *
 *   rung 1 PRIORITY        36 vs 36, TIE -- and provably symmetric: the two
 *          critical paths are 17 ->(1) 31 ->(2) 37 and 28 ->(2) 34 ->(1) 37,
 *          both length 3. The asymmetry is in the machine description's
 *          latencies, not in anything source can reach.
 *   rung 2 CLASS vs last_scheduled_insn   3 vs 3, TIE (both independent of the
 *          CHAR pool load). NOTE this rung is what makes the ldrb lose at t=2
 *          and win at t=4 -- at t=2 last_scheduled_insn is the L1940 pool load
 *          and the ldrb is class 1 on it. The trace prints the ready list
 *          WORST-FIRST, so "Ready list (t=2): 12 28 17 22" and
 *          "Ready list (t=4): 12 17 28" show the flip directly.
 *   rung 3 DEPENDENT COUNT 4 vs 3, MORE WINS -> THE ldrb TAKES THE SLOT.
 *   rung 4 INSN_LUID       never reached. This is why all 17 best orders give
 *          one stream: the decision is one rung ABOVE source order, so no
 *          statement permutation can touch it.
 *
 * The ldrb's fourth dependent is insn 37, THE STORE, and it is a MEMORY
 * ANTI-DEPENDENCE (a load before a store), not a real one. Removing it would
 * tie the count at 3 and drop the decision to INSN_LUID, where insn 17 already
 * has the lower LUID -- i.e. it would land the function. Every route to
 * removing it was measured and all are closed:
 *
 *   - `const` on the table and on the pointer local: EXACTLY INERT. gcc-2.96
 *     does not set RTX_UNCHANGING_P on the MEM for an extern const array
 *     reached through a pointer local -- verified on the dependence table
 *     itself, which is bit-identical with and without const (insn 28 still
 *     "49 48 37 34"). This is worth recording: `anti_dependence` returns 0 for
 *     an unchanging read, so const LOOKS like the exact cure and is not.
 *   - DIFFERENT_ALIAS_SETS_P: unreachable. The load must be a BYTE load to
 *     stay a `ldrb`, and every one-byte C type is a character type, which is
 *     alias set 0 and conflicts with everything. A one-byte struct member is
 *     alias set 0 too (the brief's "a struct member is inert"). Measured
 *     anyway: an 8-bit BITFIELD and a one-byte UNION member both go to 6 of 9
 *     (they change the lowering, not the dependence).
 *   - `-fno-strict-aliasing`: EXACTLY INERT at 2 of 9.
 *   - base_alias_check: cannot resolve either side. The load's base register
 *     r3 is SET THREE TIMES in this function (pool load, ldrb, shift), so
 *     init_alias_analysis records no reg_base_value for it. AND THE ROM'S OWN
 *     STREAM HAS THE SAME THREE SETS OF r3, so this is not something our
 *     spelling got wrong.
 *   - Giving insn 17 a fourth dependent (elevation.md's "A THIRD LEVER FOR THE
 *     ADJACENT PAIR"): structurally impossible here. 17 is not a memory insn,
 *     so an edge out of it needs another insn that reads or writes r1, and r1
 *     is written once and read once in the ROM's own nine instructions. The
 *     elevation.md device needs an if/else whose tail can be sunk into both
 *     arms; this function is a single basic block.
 *
 * THE OPEN QUESTION, STATED HONESTLY
 * ---------------------------------
 * With this instruction set and this register assignment -- which is the ROM's,
 * byte for byte, in all five roles -- sched2's ranking at that cycle prefers
 * the ldrb deterministically. So either the TU was built with
 * -fno-schedule-insns2, or there is a lever outside everything measured here.
 *
 * -fno-schedule-insns2 was swept across all 120 orders too
 * (RESULTS_nosched2.txt): best 2 (p_TEOKCV), histogram 1x2, 3x3, 8x4, 22x5,
 * 39x6, 44x7. So the flag does NOT land it either, and it lands nothing the
 * production flags do not -- under it the best residue is a swap of the CHAR
 * and ewram pool loads instead. THE FLAG IS NOT THE ANSWER AND SHOULD NOT BE
 * ADDED. Recorded because SCHED2_CFLAGS is an established Makefile row and
 * someone will otherwise try it: the trap is that the source order that gives
 * the ROM's ALLOCATION (T,E,O,K,C,V) and the source order that gives the ROM's
 * LUID ORDER (T,C,O,K,E,V) are DIFFERENT orders, and the latter allocates
 * CHAR->r4, off->r2, ewram->r1 (5 of 9). You cannot have both.
 *
 * DECLINING TO CLOSE. The park's diagnosis is refuted and the figure is 7 -> 2
 * with the mechanism named down to the ranker rung, but I have no proof that
 * the last swap is unreachable -- only that four named routes to it are closed.
 * This is an open park with a much better map, not a closure.
 */
extern unsigned char L1940[] __asm__(".L1940");
extern int CHAR_ARRAY_ARRAY_971__02009928[];
extern unsigned char ewram_2002224[];

/* Leaf helper: copies one word out of a six-word table into the slot that a
 * byte lookup table selects.  ewram_2002224[L1940[i]] = CHAR_..._02009928[i],
 * with both accesses reached as scaled-index-plus-base so the index lands in
 * Rn and the symbol in Rm. */
void OvlFunc_971_2008128(int i)
{
    unsigned char *t;
    int cbase;
    int ebase;
    int off;
    int k;
    int v;

    ebase = (int)ewram_2002224;
    off = i * 4;
    cbase = (int)CHAR_ARRAY_ARRAY_971__02009928;
    t = L1940;
    k = t[i];
    v = *(int *)(off + cbase);
    k <<= 2;
    *(int *)(k + ebase) = v;
}
