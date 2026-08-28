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
 */
