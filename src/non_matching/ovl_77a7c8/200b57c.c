/* OvlFunc_881_200b57c -- 0x0200b57c,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_a_c.s (SINGLE-function file)
 *
 * 84 of 84 lines, TWO differing.  Candidate at scratch/Nb57c_best.c.
 *
 * SOLVED, AND THIS IS THE PART WORTH CARRYING: A TYPED HALFWORD FIELD FIXES A
 * STORE, AND THE LITERAL POOL WITH IT.
 *
 * Written with a cast through a `char *`, `*(short *)(e + 0x64) = 0` compiles to
 * `ldr r3, =0x0` -- gcc puts a literal ZERO in the constant pool -- where the ROM
 * has `mov r3, #0`.  That extra pool entry is also what forces gcc to dump the
 * pool in the MIDDLE of the function, emitting a `b`/label pair to jump over it,
 * which the ROM does not have.  Two problems, one cause, and together they were
 * 31 differing across 86 lines against the ROM's 84.
 *
 * Declaring the object with the `struct Actor` already in this overlay
 * (src/overlays/rom_77a7c8/ovl_30_a_a_a_c_b.c) and writing `e->f64 = 0;` fixes
 * BOTH: `mov r3, #0`, the pool goes back to the end of the function, and the
 * count drops 31 -> 2 with the line count exact.  The two int stores at 0x18 and
 * 0x1c were already right either way, so it is specifically the HALFWORD that
 * needs the type.
 *
 * This extends the batch-145 finding, which was about a halfword READ that sched2
 * treated differently once it had an alias set.  It applies to STORES too, and
 * the visible symptom here is not scheduling at all -- it is a pooled constant
 * and a pool dump point.  Reach for the struct whenever a halfword access sits
 * near an unexplained `ldr rN, =<small>` or an unexplained mid-function pool.
 *
 * BLOCKER: argument-setup order at __MapActor_SetSpeed, and only there.
 *      rom   ldr r2, =0x3333 / mov r0, #0x8  / ldr r1, =0x6666
 *      ours  ldr r2, =0x3333 / ldr r1, =0x6666 / mov r0, #0x8
 * `mov r0` wants to sit BETWEEN the two pool loads.  That is the "r0 into the
 * middle" half of the batch-147 pair, whose lever is naming the constant in a
 * block that DOMINATES the call -- and this function is straight-line, so there
 * is no such block.  Naming both speed constants at the top costs seven lines
 * (91 vs 84); naming only the y constant costs one and 59 differing.
 *
 * SCREENED AND UNCHANGED AT 2, so nobody repeats them: the slot 8 as a named
 * local; `0x6666` written as `0x3333 << 1`; unsigned parameter types; an
 * `__asm__` alias for the callee returning int, `char *`, or `long long`;
 * deleting the callee's declaration entirely; and the per-declaration greedy
 * sweep over every other callee (tools/protolever.py).
 *
 * FLAGS, all screened: -fno-schedule-insns (2, unchanged), -fno-strength-reduce
 * (2), -fno-strict-aliasing (2), -ffixed-r7 (2), -fno-rerun-cse-after-loop (2),
 * -fno-gcse (2).  -fno-schedule-insns2 and -O1 both go to 26 differing, so
 * sched2 is producing the ROM's order everywhere else in the function and this
 * one site is not a reason to turn it off.
 */
