/* Func_80b84c0 -- asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s
 *
 * BLOCKER: REGISTER ROLES at a TWO-SITE .call_via -- 42 of 48, SAME LENGTH
 *
 * CORRECTED.  The numbers below the line were measured against a tryc.py that
 * SILENTLY DROPPED the `.call_via` line from the ROM stream (it starts with a
 * dot, so the ref parser skipped it as a directive, but it is a macro from
 * include/macros.inc expanding to two real instructions).  The ROM was never
 * 48 lines against our 54 -- it is 48 against 48.  "Six instructions too many"
 * was an artefact of the screen, and the real gap was two.
 *
 * With the parser fixed and two further levers, scratch/agent4/sb84c0_k.c is
 * 48 lines against 48 with 42 differing -- one register-role exchange, no
 * length difference at all:
 *
 *   * Bind the function pointer at FUNCTION scope
 *     (`register int (*f)(int,int) __asm__("r5");`) and reference it as an
 *     "r" input of a bare __asm__ at each site.  Passing it INTO a helper
 *     whose parameter is register-bound forces a second pseudo: gcc parks the
 *     pointer in r10 and emits `mov r5, r10` at every site.
 *   * A volatile asm is NOT cross-jumped.  Writing the call in both arms of an
 *     if/else gives two expansions.  Select only the differing argument into a
 *     local (`if (...) k = 0x18; else k = 0x30;`) and put ONE asm after the
 *     join.
 *
 * Residue: the ROM spends four callee-saved registers, we spend five, because
 * gcc will not coalesce the call result into r6 after the earlier value dies.
 * Measured neutral: -ffixed-r10, -ffixed-sl, -fno-strict-aliasing, -fno-gcse,
 * -fno-rerun-cse-after-loop -- all byte-identical to no flag.  Note -ffixed-r10
 * being a complete no-op is itself a finding: gcc-2.96 thumb never allocates
 * r10 by choice, so reserving it changes nothing.
 *
 * ---- superseded, kept for the record ----
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
