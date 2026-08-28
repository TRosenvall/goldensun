/* Func_80b84c0 -- asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s
 *
 * BLOCKER: ONE EXTRA CALLEE-SAVED REGISTER at a TWO-SITE .call_via
 *
 * 53 of 48, ours 54 lines (six too many).  The body aligns instruction for
 * instruction after a one-line prologue offset -- the semantics are right.
 *
 * This is the .call_via class, which is REACHABLE (see Func_8097a10 in
 * src/rom_8a000/rom_97384_c_c_a_b.c for the working inline-asm helper and
 * docs/elevation.md for the retraction of the "hard wall" claim).  What is
 * different here is that the function calls through the SAME pointer TWICE:
 *
 *     rom  r5 = actor ptr ... PhysMove ... ldr r5, =Func_8000888
 *          .call_via r5   ...   .call_via r5
 *
 * The ROM REUSES r5 -- actor pointer first, function pointer second, born only
 * after PhysMove -- and spends four callee-saved registers (r5 r6 r7 r8).  We
 * spend five (adding r10), because the function pointer is live from where the
 * source names it, and that pushes the second argument out of r7.
 *
 * MEASURED (all ours-54 against rom-48):
 *   `f = Func_8000888;` local, assigned immediately before the first site   53
 *   the address written inline at BOTH sites instead of a local             53
 *
 * The one-site template transfers cleanly; the two-site shape needs the
 * pointer's live range to START after the intervening call, and neither
 * spelling above achieves that.  Likely levers not yet tried: a
 * function-scope `register ... __asm__("r5")` binding (risky -- r5 also holds
 * the actor pointer earlier, so the binding would have to be introduced late),
 * or two separate locals if gcc will still CSE the two pool loads into one.
 *
 * Best C: scratch/sb84c0.c.
 */
