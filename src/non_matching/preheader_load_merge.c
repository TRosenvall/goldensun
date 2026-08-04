/* THE PRE-HEADER LOAD MERGE -- a blocker class, not a function.
 *
 * This file documents a shape that three functions now sit on. It is not built
 * and defines nothing; read it before attempting a fourth.
 *
 * THE SHAPE
 *
 * A spin-wait loop, un-rotated: the value is read once before the loop and
 * again at the bottom of the body, and both reads reach the same test block.
 *
 *     rom    ldr r3, =SYM / ldr r3, [r3] / mov r5, #0 / b .Lcheck
 *            .Lloop: ... / ldr r3, =SYM / ldr r3, [r3]
 *            .Lcheck: cmp r3, #0 / bne .Lloop
 *
 *     ours   ldr r3, =SYM /                mov r5, #0 / b .Lcheck
 *            .Lloop: ... / ldr r3, =SYM
 *            .Lcheck: ldr r3, [r3] / cmp r3, #0 / bne .Lloop
 *
 * gcc CROSS-JUMPS: the two predecessor blocks end in the same instruction, so
 * it sinks that instruction into the shared successor and saves one. Every
 * member is short by exactly one instruction, and it is always this one.
 *
 * The goto-loop lever from docs/elevation.md is a prerequisite, not a fix. It
 * gets the loop skeleton right -- `while` gives the rotated shape and is
 * several instructions further out -- and then this is what remains.
 *
 * MEMBERS
 *
 *   Func_80064b8         24 of 25   src/non_matching/rom_c0/rom_64b8.c
 *   Func_8012350         26 of 27   src/non_matching/rom_9000/rom_12350.c
 *   OvlFunc_956_20081c8  25 of 26   src/non_matching/ovl_7e0928/20081c8.c
 *
 * It is the same defect in the same direction as constant-CSE -- gcc doing
 * more than Camelot's compiler did -- but the mechanism is different, so the
 * two should not be filed together. constant-CSE is about a value gcc refuses
 * to rebuild; this is about an instruction gcc refuses to duplicate.
 *
 * TRIED, ACROSS THE THREE
 *
 *   1. `volatile` on the loaded field. This is the attempt that looks most
 *      promising and it does NOTHING, for a reason worth writing down:
 *      cross-jumping RELOCATES the read, it does not remove one. Exactly one
 *      read still happens per pass, so the volatile semantics are satisfied
 *      either way and gcc is free to merge.
 *   2. duplicating the test in the source so the two reads reach different
 *      blocks -- overshoots to 29 against 25 on Func_80064b8; gcc keeps both
 *      copies AND adds a branch.
 *   3. reading through a pointer local rather than the global directly -- the
 *      address load then hoists out of the loop entirely, which is further
 *      from the ROM, not closer.
 *   4. the loaded value in two separate locals, one per read site. gcc
 *      coalesces them; this is the same negative recorded in
 *      docs/elevation.md, that separate variables do not defeat a copy.
 *
 * THE FLAG PROBE WAS TRIED AND FAILED
 *
 * The obvious next step was a per-file compiler flag, the way the tree already
 * carries per-file -O1 rules for genuine per-TU differences in the original
 * build. It does not work:
 *
 *   -fno-crossjumping          cc1: Unrecognized option. It does not exist in
 *                              gcc-2.96 -- it arrives in gcc 3.4. Cross-jumping
 *                              here is part of jump.c and has no off switch.
 *   -fno-thread-jumps          accepted, and byte-identical output. Not the
 *                              pass responsible.
 *   -O1                        rotates the loop instead, which is further out.
 *
 * So there is no flag to reach for, and that closes the line of attack rather
 * than leaving it open. What remains is either a C form nobody has found or
 * the permuter.
 *
 * A NOTE ON WHY THIS IS WORTH THREE PARKS RATHER THAN THREE GUESSES
 *
 * Each member is one instruction short, which is exactly the range where it is
 * tempting to keep permuting the source until something lands. All three are
 * short by the SAME instruction for the SAME reason, so a formulation that
 * fixes one should fix all three -- and that is the test to apply to any future
 * candidate fix before believing it.
 */
