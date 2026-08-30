/* OvlFunc_955_2009424 -- 0x02009424, asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c.s
 *
 * 108 of 108 lines in the best spelling, 78 differing.
 * Candidate: scratch/H9424_best.c.
 *
 * A NEAR-TWIN OF THE ELEVATED OvlFunc_954_20095e0
 * (src/overlays/rom_7db0c8/ovl_30_c_c_c_c_b.c) for its first third: same gState
 * guard, same OvlFunc_common1_4cc dispatch, same three-way result handling.  The
 * bodies then diverge completely -- different callees, not a constant swap --
 * so the elevated sibling is a template for the frame and nothing more.
 *
 * BLOCKER: REGISTER PRESSURE.  The ROM pushes {r5, r6, r7, lr} and holds exactly
 * three values -- the parameter in r6, the dispatch result in r7, and 0x87/0x438
 * in r5.  Every spelling tried wants a FOURTH callee-saved register and emits
 * `mov r7, r8 / push {r7}`, which the ROM does not have.
 *
 * The four values compete and only three fit.  Screened:
 *      named gState base + named 0x438      111 lines, 97 differing
 *      base INLINED, 0x438 named            108 lines, 78   <- best
 *      named offset instead of named base   111 lines, 97
 *      named base, 0x438 written inline     111 lines, 97
 *
 * Naming the base is what stops gcc folding `ldr r3, =gState+450`, which the ROM
 * does not do -- but naming it is also what forces the extra register.  The two
 * requirements are in direct conflict and no arrangement satisfies both.
 *
 * RETRIED with the offset-naming lever found on OvlFunc_932_200a310 -- name the
 * OFFSET rather than the base, which satisfies both halves of that tension
 * elsewhere.  It does NOT help here: 111 lines and 97 differing, against the 108
 * and 78 of the inlined form.
 *
 * The reason is that this park's note overstated the similarity.  On 200a310 the
 * inlined base FOLDS and costs three instructions; here the inlined base gives
 * MATCHING length already, so the fold was never what this function was losing
 * to.  The 78 are register pressure and nothing else, and the third arrangement
 * has nothing to fix.  Recorded so the next reader does not retry it a third
 * time.
 *
 * NOTE the contrast with the sibling, which needed the base named AND has the
 * same construct: it gets away with it because it holds only two values, so the
 * third register is free.  The lever is not wrong there and right here; the
 * register budget differs.  That is worth remembering before copying a spelling
 * from a twin -- what a template proves is the SHAPE, not that its levers fit.
 */

/* RETRIED (batch 147) WITH THE PROTOTYPE LEVER, and it helps but does not
 * close it: 78 differing -> 74, best candidate now scratch/H9424_best.c.
 * OvlFunc_common1_1078, OvlFunc_common1_15b8 and OvlFunc_common1_5e4 are left
 * undeclared, which puts `mov r0` at the end of their argument setup exactly as
 * it does on the sibling OvlFunc_955_20092f0 (now elevated) and on
 * OvlFunc_956_200a4d0.  Keep them undeclared in any future attempt.
 *
 * THE REGISTER-PRESSURE NOTE ABOVE NAMED THE WRONG FOURTH VALUE.  It is not the
 * gState base and it is not 0x438.  It is `-1`:
 *
 *      ours  mov r5, #0x1 / bl __Func_80933d4 / neg r5, r5 / ... / mov r1, r5
 *      rom   mov r1, #0x1 ... neg r1, r1          (built at the call, discarded)
 *
 * The function calls __Func_80933f8 twice, once with a single -1 and once with
 * __Func_80933f8(-1, -1, -1, 0).  Four uses of a TWO-INSTRUCTION constant, so
 * gcc common-subexpressions them into one pseudo, and because that pseudo is
 * live across __Func_80933d4 it must be callee-saved -- which is the `mov r7, r8
 * / push {r7}` the earlier note read as the naming lever's fault.  It is not.
 * With the base inlined the push is still there.
 *
 * That makes this function a member of the class in
 * src/non_matching/overlays/constant_reuse.c and the `-1` triple specifically:
 * every function in the tree containing __Func_80933f8(-1, -1, -1, 0) is parked,
 * none is elevated, and an eleven-flag sweep against that class found nothing.
 * Do not spend another round on this one until that class breaks.
 *
 * For the record, naming the gState base DOES fix the address fold -- with it
 * the entry is `ldr r3, =gState / mov r2, #0xe1 / lsl r2, #1 / add r3, r2`, the
 * ROM's spelling, instead of a folded `ldr r3, =gState+450`.  It costs three
 * lines elsewhere (111 vs 108) only because the -1 pseudo is already consuming
 * the register budget.  Fix the -1 first and the base naming should come free.
 */
