/* OvlFunc_911_20082b4 -- 0x020082b4,
 * asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_a.s
 *
 * 33 of 33 lines, 5 differing.  Candidate at scratch/L82b4.c.
 *
 *      rom   lsl r2, #0x7 / mov r0, #0x0 / lsl r1, #0x8
 *      ours  lsl r2, #0x7 / lsl r1, #0x8 / mov r0, #0x0
 *
 * THIRD instance of the argument-setup-order class, after
 * src/non_matching/overlays/20099a4.c and 2008a80.c.  Everything else is exact.
 *
 * What the three together establish: gcc ALWAYS emits the `mov r0, #0` last in
 * the argument-setup block, and the ROM never does.  Across the three the ROM
 * places it in two different slots -- before both shifted builds in 20099a4,
 * between them in 2008a80 and here -- so there is no fixed convention to
 * imitate, and `--no-sched2` makes all three worse, so the ROM was built with
 * the second scheduling pass running.  The difference is in what that pass was
 * handed, which is the argument evaluation order, and nothing tried reaches it.
 *
 * TRIED across the three: naming the zero as a local; sharing one zero local
 * across the calls that take one; naming the shifted values as locals;
 * --no-sched2; -fno-schedule-insns; -fno-defer-pop; -fomit-frame-pointer.
 *
 * The class is now sized -- see docs/elevation.md.  It is worth detecting
 * BEFORE writing a candidate, because a function whose only defect is this is
 * otherwise indistinguishable from one that is nearly right for a reachable
 * reason.
 *
 * CORRECTION (added later, and it changes the class).  An earlier version of
 * this park said gcc ALWAYS emits the `mov r0, #0` last in an argument block.
 * That is wrong.  A control over the 3411 solved functions in this tree found
 * 15 that carry the exact interleaved shape, one of them the identical
 * `mov r1,#128 / mov r2,#128 / mov r0,#0 / lsl r1,#9 / lsl r2,#8 /
 * bl __MapActor_SetSpeed`.
 *
 * The construct is in src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_b.c: the
 * shifted constants are NAMED LOCALS assigned at the top of the function, and
 * -- this is the part that matters -- TWO EARLY RETURNS sit between the
 * assignments and the calls.  gcc will not keep seven constants live across the
 * guards, so it rematerialises each at its use, and the rematerialised sequence
 * interleaves the way the ROM's does.
 *
 * That is why the lever does not reach THIS function: it is straight-line.
 * Naming the constants at the top with no branch in between makes gcc keep them
 * live instead, which is worse, not better (33 -> 39 lines on
 * OvlFunc_911_20082b4, 26 -> 29 on OvlFunc_899_20099a4).
 *
 * So the blocker here is not "gcc cannot produce this order" but "this function
 * has no dominating block to produce it from".  Sized in docs/elevation.md:
 * 248 remaining functions carry the shape, 150 of them have a conditional
 * branch before the site and are worth trying the lever on; 98, including this
 * one, are straight-line at every site.
 */
