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
 */
