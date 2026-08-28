/* OvlFunc_899_20099a4 -- 0x020099a4,
 * asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_a_a.s
 *
 * Best screen: 26 of 26 lines, 5 differing -- and all five are the position of
 * ONE instruction, `mov r0, #0x0`, in each of the two calls that take a zero
 * first argument.  Candidate at scratch/L99a4.c.
 *
 *      rom   mov r1,#0x80 / mov r2,#0x80 / mov r0,#0x0 / lsl r1,#8 / lsl r2,#7
 *      ours  mov r1,#0x80 / mov r2,#0x80 / lsl r1,#8 / lsl r2,#7 / mov r0,#0x0
 *
 * BLOCKER: argument-setup order.  The ROM emits the zero between the two
 * shifted-constant bases and their shifts; gcc emits it after both shifts.
 *
 * Everything else is exact, including both shifted builds in both calls and the
 * whole store sequence.  That store is itself a useful confirmation: the ROM has
 * `add r3, r2 / mov r2, #0x10 / str r2, [r3]` -- the address IS materialised
 * here rather than folded into a reg+reg store, and the reason is visible, since
 * r2 holds the offset and is then reused for the stored value.  It reproduces
 * from the plain integer-local chain with no coaxing.  That is direct evidence
 * for what src/non_matching/overlays/200808c.c concludes: whether gcc
 * materialises an address or folds it is decided by register pressure, not by
 * how the source is spelled.
 *
 * TRIED: naming the zero as a local shared by both calls; naming the two shifted
 * values as locals before the call; --no-sched2 (10 differing, worse);
 * -fno-schedule-insns, -fno-defer-pop, -fomit-frame-pointer (all 5).
 *
 * The basic-block lever from batch 119/123 does not apply: it works by naming
 * constants in a DOMINATING block, and this function is straight-line with no
 * branch to supply one.  A straight-line call script whose only defect is the
 * position of an r0 constant has no lever at present.
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
