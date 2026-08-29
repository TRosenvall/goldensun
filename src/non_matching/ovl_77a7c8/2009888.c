/* OvlFunc_881_2009888 -- asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * BLOCKER: CONSTANT CSE of three `-1` arguments, NO BOUNDARY -- 59 of 61
 *
 * THREE-MEMBER FAMILY, identical shape, all in the same .s:
 *   OvlFunc_881_2009888, OvlFunc_881_2009938, OvlFunc_881_20099e8
 * All three open `push {r5, lr} / GetActor(8) / CutsceneStart` and then call
 * `__Func_80933f8(-1, -1, -1, 0)` with nothing between the function entry and
 * the call.  None of them can take the lever below.
 *
 *     rom  mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0 / neg r1 / neg r2 / neg r0
 *     ours mov r5,#1 / neg r5,r5 / ... / mov r1,r5 / mov r2,r5 / mov r0,r5
 *
 * gcc builds `-1` once and copies it three times, and the copy costs an extra
 * callee-saved register (`push {r5, r6, lr}` against the ROM's `{r5, lr}`).
 *
 * THIS IS THE CONTRAST CASE FOR src/overlays/rom_7aa430/ovl_1150_c_c_b.c.
 * That function -- OvlFunc_923_2009208 -- calls the SAME callee with the SAME
 * three `-1` arguments and MATCHES, using three separate `int n1, n2, n3 = -1`
 * locals.  The difference is not the spelling: 2009208 has an early
 * `if (__GetFlag(0x109)) return;` between the assignments and the uses, so the
 * assignments sit in a block that DOMINATES the call without being the call's
 * own block.  Here there is no guard, the whole prologue is one basic block,
 * and the identical C is CSEd.
 *
 * MEASURED: three separate `int` locals assigned at the top -- 59 of 61,
 * ours 62 lines.  That is the lever applied correctly and defeated by the
 * absence of a boundary, which is exactly what the rule predicts.
 *
 * Best C: scratch/G9888.c.  The other two members differ only in the
 * behaviour script and the flag id.
 */
