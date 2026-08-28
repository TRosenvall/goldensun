/* OvlFunc_955_2009424 -- 0x02009424, asm/overlays/rom_7ddb88/ovl_30_c_c_c_c.s
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
