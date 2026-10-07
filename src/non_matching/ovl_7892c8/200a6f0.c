/* OvlFunc_888_200a6f0 (0x0200a6f0) -- NON-MATCHING.
 *
 * NON-MATCHING, 2 of 46 encodings  (MEASURED, batch 334, per-function and
 *   whole-file views agreeing).  INSTRUCTION COUNTS NOW AGREE -- ref 43,
 *   ours 43, SIZE 96/96.
 *   The previous figure, thirty-one of forty-six, was POSITIONAL on a stream
 *   two instructions short; this one is a distance, and the residue is SCHED2
 *   ORDER ONLY.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7892c8/200a6f0.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a_a.s \
 *     --func OvlFunc_888_200a6f0
 *
 * Pin-free. The piece holds this function alone.
 *
 * ============ BATCH 334: THE OLD VERDICT WAS WRONG ============
 *
 * The old header's register table was exactly right:
 *
 *              the angle    the +0x64 pointer
 *     rom      r6 (low)     r8  -- so it costs `mov r2, r8` and `mov r1, r8`
 *     ours     r8           r6 (low) -- no moves needed
 *
 * Its verdict -- "this is not a spelling problem, our code is better and there
 * is nothing to write. NEXT: nothing source-level" -- was not. There is.
 *
 * THE LEVER FOR THE PAIR: DECLARE THE ANGLE `unsigned short`, NOT `int`.
 * That alone takes it from ref 43 against ours 41 to 43/43 and eighteen of
 * forty-six. The old probes could not find it because all three varied the
 * POINTER's spelling, and the decision is made on the ANGLE's MODE.
 *
 * Why the mode decides it: this function has no branches, so it is ONE basic
 * block and global-alloc has nothing to do -- its own dump prints zero regs to
 * allocate, and the entire assignment is local-alloc's. local-alloc orders
 * quantities by QTY_CMP_PRI (local-alloc.c:1496-1498), which is
 * floor_log2(n_refs) * n_refs * size / (death - birth), and in Thumb it can
 * never hand out r7 (arm.h:898-899), so there are exactly TWO callee-saved low
 * slots, r5 and r6, for FOUR call-crossing values: the actor, the other
 * pointer, the angle and the +0x64 pointer. As an `int` the angle is SImode and
 * cheap in a high register, so the +0x64 pointer (four refs) outranks it and
 * takes r6. As `unsigned short` it is HImode, thumb_movhi_insn makes the high
 * registers dear, the angle takes r6, the pointer is pushed to r8 -- and reload
 * then inserts the ROM's two `mov` retrievals, which ARE the missing pair.
 *
 * `short ang` is NOT the same: signed reads ref 43 against ours 42 and
 * thirty-nine of forty-six, because the sign-extend appears.
 *
 * ============ THE OTHER THREE ONLY PAY CROSSED ============
 *
 * Measured one at a time, one of them looks WORSE. Crossed, they take it to 2.
 *
 *   * reading the other pointer BEFORE the +0x64 pointer swaps the r1/r2 head
 *     assignment. Eighteen to twelve, crossed with the next.
 *   * the sin accumulate written load-first -- the same shape as the cos line,
 *     so both accumulate into the LOADED register. ALONE THIS IS WORSE: twelve
 *     where the reversed spelling reads eight. Crossed with the temp below it
 *     reads TWO. One-at-a-time testing rejects the right answer here, which is
 *     the search-discipline failure the briefs keep naming.
 *   * a named temp for the `a+8` reload, read BEFORE the two v stores. Eight to
 *     seven on its own, and it is what lets the sin spelling pay.
 *
 * ============ THE RESIDUE IS 2 AND IT IS SCHED2 ============
 *
 * The ROM orders `str r3, [r5, #8]`, then the argument move, then the call; we
 * emit the two in the other order. Both must precede the call -- one is memory,
 * one is the argument -- so it is a rank_for_schedule tie-break, not a value
 * and not a register. arm_adjust_cost (arm.c:2416) puts the argument move at
 * exactly prio(call)+1; the store's chain runs on through the later reload of
 * `a+8`. Six orderings of the cos block and the tail were measured: the temp
 * read before the v stores is the only one at 2, reading it in place is twelve,
 * and storing `a+0x38` before the v stores is four.
 */
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_888_200a6f0(unsigned char *a)
{
    unsigned char *q;
    unsigned char *b;
    unsigned short ang;
    int c;
    int s;
    int v;
    int t;

    b = *(unsigned char **)(a + 0x68);
    q = a + 0x64;
    ang = *(unsigned short *)q;
    c = __cos(ang);
    *(int *)(a + 8) = *(int *)(b + 8) + ((c * 8 - c) << 1);
    s = __sin(ang);
    v = *(int *)(b + 0x10) + ((s * 4 + s) << 1);
    t = *(int *)(a + 8);
    *(int *)(a + 0x10) = v;
    *(int *)(a + 0x40) = v;
    *(int *)(a + 0x38) = t;
    a += 0x66;
    *(unsigned short *)q = *(unsigned short *)q + *(unsigned short *)a;
}
