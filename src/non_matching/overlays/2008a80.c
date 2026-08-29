/* OvlFunc_901_2008a80 -- 0x02008a80,
 * asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c.s
 *
 * Best screen: 30 of 30 lines, TWO differing.  Candidate at scratch/L8a80.c.
 *
 *      rom   lsl r1, #0x8 / mov r0, #0x0 / lsl r2, #0x7
 *      ours  lsl r1, #0x8 / lsl r2, #0x7 / mov r0, #0x0
 *
 * Same blocker as src/non_matching/overlays/20099a4.c: the position of the
 * `mov r0, #0` that sets up a zero first argument.  Everything else is exact,
 * including both shifted builds and the whole integer-local address chain for
 * the iwram store.
 *
 * This is the second instance, and together they narrow the class usefully.
 * The two functions place that instruction in DIFFERENT slots --
 * 20099a4 has it before both shifts, 2008a80 has it between them -- so it is
 * not a fixed convention being missed.  It is the scheduler, and the input
 * ordering it works from differs with the surrounding register pressure (this
 * function has three parameters to preserve, that one has none).
 *
 * TRIED: naming the zero as a local shared by both calls that take one (2, no
 * change); --no-sched2 (13, much worse, and it disturbs the shifted builds).
 *
 * Since --no-sched2 makes it worse rather than better, the ROM was built WITH
 * the second scheduling pass and the difference is in what that pass was
 * handed, not whether it ran.  That is a narrower statement than the 20099a4
 * park could make on its own.
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
