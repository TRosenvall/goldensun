/*
 *
 * NON-MATCHING, 9 of 57 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7d0e88/200a1ac.c \
 *     asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_a.s --func OvlFunc_947_200a1ac
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * OvlFunc_947_200a1ac -- asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_a.s
 *
 * BLOCKER: register allocation for a rematerialised constant. 54 lines
 * against 54, 9 differing, and the first difference is placement:
 *
 *      rom   mov r4, #0x8 / and r3, r1 / orr r3, r4
 *      ours  and r3, r1  / mov r1, #0x8 / orr r3, r1
 *
 * The ROM materialises the OR constant into r4 BEFORE the AND and keeps it
 * live across both flag updates; we rematerialise it into a scratch register
 * at each use. The two updates then diverge on which register holds the actor
 * pointer.
 *
 * SETTLED, and this one is a NEW LEVER worth reusing:
 *
 *   A MASK APPLIED TO A BYTE GETS NARROWED UNLESS IT IS NAMED. The ROM builds
 *   -13 as `mov r2, #0xd / neg r2, r2` -- two instructions, the full 32-bit
 *   value. Written inline as `p[9] = (p[9] & -13) | 8;` gcc notices the result
 *   is stored back into a byte, truncates the mask to 0xf3, and emits a single
 *   `mov r2, #0xf3`. That is one instruction SHORT and it cascades.
 *
 *   Assigning the mask to an `int mask = -13;` first stops the narrowing and
 *   gives the ROM's mov/neg pair. 53 lines and 42 differing becomes 54 and 9.
 *
 *   An `int` intermediate for the LOADED BYTE (`t = p[9]; p[9] = (t & -13) | 8;`)
 *   does NOT work -- 15 differing. It is the mask that has to be named, not the
 *   value being masked.
 *
 * TRIED AND REJECTED, all measured, all identical at 9 differing:
 *
 *   * `m = 8;` assigned BEFORE `mask = -13;`
 *   * `mask` declared before `m`
 *   * the OR constant left as a bare literal 8 rather than a named local
 *   * `unsigned char m = 8;` -- the narrow type the ORR-destination lever
 *     prescribes. MUCH WORSE: 47 differing and 55 lines, first difference at 8
 *     instead of 20. Tried because that lever closed OvlFunc_946_20092b4 and
 *     OvlFunc_903_2008d68 on a residue that looks identical to this one. It
 *     does not transfer: there the constant is the ORR DESTINATION and here
 *     the ROM's problem is that the constant must stay LIVE across two flag
 *     updates, which a narrow local does not do. The doc's caution that the
 *     two operations must be tried separately is the right reading.
 *
 * Naming the OR constant changes nothing in either direction, which is itself
 * the answer to the obvious next idea.
 *
 * Also settled: the actor pointer must come from a local assigned from the
 * call (`p = __MapActor_GetActor(0xd);` then `*(int *)(p + 0x18) = v;`), not
 * from the call embedded in the store expression. The embedded form swaps the
 * r5/r6 roles of the two long-lived values and moves the first difference from
 * line 16 to line 4.
 

 *
 * ===== BATCH 329 BRIEF I: RE-DERIVED AT 9, AND THE PARK'S DIAGNOSIS IS WRONG =====
 *
 * Re-measured unfiltered: 9 differing encodings of 57, ref 57 ours 57, first at
 * index 20, no SIZE and no POOL WORD line, so the streams are aligned and this
 * is a distance and not the padding trap.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7d0e88/200a1ac.c asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_a.s --func OvlFunc_947_200a1ac
 *
 * REFUTED: the header says "we rematerialise it into a scratch register at each
 * use". We do not. There is exactly ONE `mov r1, #0x8` and it is reused at both
 * `orr` sites, which is structurally what the ROM does with r4. The difference
 * is not HOW MANY TIMES the constant is built, it is WHERE ITS LIVE RANGE
 * STARTS.
 *
 * DECOMPOSITION, 9 in three runs, and runs A and B are ONE cause:
 *   A  3 insns, index 20-22. rom `mov r4, #0x8 / and r3, r1 / orr r3, r4`
 *      against ours `and r3, r1 / mov r1, #0x8 / orr r3, r1`.
 *   B  4 insns. the second sprite pointer, rom r1 against our r0 -- forced,
 *      because our constant took r1.
 *   C  2 insns. `mov r0, r6` against `lsl r2, #0xe`, the TravelTo argument
 *      setup, downstream of nothing in A/B and the only independent run.
 *
 * THE MECHANISM, read in the compiler and confirmed in the dumps:
 *   `arm.h:989-995` REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,6,7,...}; r12 and r14
 *   are HI_REGS and cannot hold a thumb `orr` operand, so the usable order here
 *   is 3,2,1,0,4,5,6,7 -- r4 is reached ONLY when r3, r2, r1 and r0 are all
 *   busy for the WHOLE of the pseudo's live range.
 *   Over the ROM's span (from before the first `and` to the second `orr`) they
 *   are: r3 the accumulator, r2 the mask, r1 the loaded byte, r0 the first
 *   sprite pointer. Hence r4.
 *   Over OUR span (from AFTER the first `and`) r1 has just been freed by the
 *   `and`, so r1 wins on order and the `mov` is then pinned after the `and` by
 *   its own anti-dependence, which is why sched2 cannot hoist it the way the
 *   ROM's build did.
 *   In `.17.lreg` the constant is `(reg:QI 46)` -- QI MODE, because the result
 *   is stored through a byte -- set at insn 59, BETWEEN the `and` (insn 55) and
 *   the `ior` (insn 61), and `;; Register 46 in 1.`. The QI narrowing is what
 *   relocates the definition point. That, not "allocation", is the blocker.
 *
 * SO THE QUESTION IS SINGULAR: how does the constant's live range begin before
 * the `and`. The park's rejected list varies where `m` is assigned, the
 * declaration order, literal against local, and the type of `m` -- five rows,
 * all one dimension. The following are new dimensions and all measured:
 *
 *   named `int r` for the field-edit result, `r = (p[9] & mask) | m; p[9] = r;`
 *                                           WORSE, 39 of 57 at 55 lines -- the
 *                                           int result lets the mask re-narrow
 *   `(p[9] | m) & mask`, algebraically identical since 8 & ~0xc == 8
 *                                           WORSE, 40 of 57 at 55 lines
 *   `m | (p[9] & mask)`                     inert, 9
 *   `(mask & p[9]) | m`                     inert, 9
 *   one-member `union { int i; }` for the OR constant
 *                                           inert, 9
 *   both sprite pointers loaded before the first edit, so a second pointer is
 *   live across it (the "change which pseudos are LIVE" move)
 *                                           WORSE, 46 of 57
 *   the field edit as a 2-bit BITFIELD store, `s->b2 = 2` -- see below
 *                                           WORSE, 11 of 57, and run A is
 *                                           UNCHANGED by it
 *
 * THE EXISTENCE PROOF, AND WHY IT DOES NOT REACH THIS FUNCTION. A
 * generated-against-hand census of the residue shape over all of `asm/`
 * (4,468 gcc-generated files, 740 hand-written) finds the ROM's hoisted form
 * `mov rD,#K / and rA,rB / [lsl rD,#N] / orr|add rA,rD` TWICE in gcc's own
 * output, against 66 instances of our reused form. So the shape is REACHABLE,
 * and one of the two hits is this function's construct line for line:
 * `src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c_a.c` writes
 * `__MapActor_GetActor(0)->f50->b2 = 1;` and gets
 * `mov r5,#13 / ldrb r2,[r1,#9] / neg r5,r5 / mov r3,r5 / mov r6,#4 /
 *  and r3,r2 / orr r3,r6 / strb r3,[r1,#9]`.
 * But there the constant is in r6 and it is there because THREE such edits are
 * separated by CALLS, so the constant's range crosses a call and must take a
 * call-saved register -- which is free from the top of the function and lets
 * the `mov` sit anywhere. This function's two edits have NO call between them,
 * and r4 is call-USED in this build (-fcall-used-r4), so that route is not
 * available here. Measured: the bitfield spelling gives 11, and leaves run A
 * exactly as it was.
 *
 * WHAT WOULD CLOSE IT: anything that keeps r1 occupied across the first `and`
 * WITHOUT adding an instruction. Every candidate tried either added a live
 * pointer (46) or changed the arithmetic (39/40). Not closed.
*/
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_947_200a1ac(void)
{
    unsigned char *a;
    unsigned char *p;
    int v;
    int m;
    int mask;
    int s1;
    int s2;

    a = __MapActor_GetActor(0xe);
    p = __MapActor_GetActor(0xd);
    v = 0x80 << 9;
    *(int *)(p + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xd) + 0x1c) = v;
    p = *(unsigned char **)(__MapActor_GetActor(0xd) + 0x50);
    mask = -13;
    m = 8;
    p[9] = (p[9] & mask) | m;
    p = *(unsigned char **)(a + 0x50);
    p[9] = (p[9] & mask) | m;
    *(int *)(a + 0x34) = 0x6666;
    *(int *)(a + 0x30) = 0xcccc;
    __Actor_TravelTo(a, *(int *)(a + 8), 0x80 << 14, *(int *)(a + 0x10));
    __MapActor_WaitMovement(0xe);
    s1 = 0x16;
    s2 = 0x10;
    __Func_8010704(0x14, 0xe, 1, 1, s1, s2);
}
