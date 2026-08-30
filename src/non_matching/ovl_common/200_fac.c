/* OvlFunc_common1_fac -- asm/overlays/common/common1_a_a_a_a_c_c_a_c.s
 *
 * 75 lines against the ROM's 77, and the two-line gap is ONE expression.
 * Candidate at scratch/Nfac_best.c.
 *
 * SOLVED, three things, all reusable:
 *
 *   - THE TWO SCRIPT-TABLE OPERANDS ARE `.L3` AND `.L13`, and the tree already
 *     has the answer: `_TBL_L3` / `_TBL_L13`, absolute assignments in the three
 *     overlay linker scripts that carry this object.  gcc numbers its own local
 *     labels .L1, .L2, .L3, ... from one, so a compiled function that needs
 *     three labels DEFINES .L3 itself and an `__asm__(".L3")` extern binds to
 *     that branch target instead -- no error, just a pool word pointing at the
 *     wrong place.  Confirmed here by reading the generated .s: `.L3:` at line
 *     27 and `.word .L3` in the pool at line 90.  Use the aliases.
 *   - NAME THE TABLE BASE IN A LOCAL.  `*(short *)(_TBL_L13 + 0x1a) = v` folds
 *     to `ldr r3, =_TBL_L13+26 / strh r3, [r3, #0]`; through a named pointer it
 *     is `ldr r2, =_TBL_L13 / strh r6, [r2, #0x1a]`, the ROM's form.
 *     61 differing -> 50.
 *   - OvlFunc_common1_e10 WANTS `mov r0` LAST, so it is deliberately undeclared.
 *
 * BLOCKER: CSE ACROSS THE TWO ARMS OF THE BRANCH.  `a * 60` is computed before
 * the test and kept in r6; the positive arm reuses it (`mov r0, r6 / add r0,
 * #0x3c`) and we reproduce that.  The negative arm wants `a * -60`, and the ROM
 * BUILDS IT AGAIN from scratch -- `lsl r0, r5, #4 / sub r0, r5, r0 / lsl r0, #2`
 * -- while gcc here notices it already has `a * 60` in r6 and emits `neg r0, r6`.
 * One instruction against three, which is the whole two-line gap.
 *
 * gcc chose the THREE-instruction form in the original build with the value
 * sitting in a register, so its cost model did not relate the two expressions.
 * Nothing said in C changes that here.  SCREENED, all identical at 50:
 * `a * -60`, `-(a * 60)`, `-60 * a`, `0x3c - a * 60`, the product assigned to a
 * local first, `m * 60` off the abs value (55, worse -- it computes from the
 * wrong register), and dropping the named `a * 60` so both arms write it
 * inline.  `((a - (a << 4)) << 2)` spelled out is WORSE: gcc then CSEs the
 * `a << 4` instead and spends a fourth callee-saved register on it (76 lines).
 *
 * FLAGS: -fno-rerun-cse-after-loop, -fno-expensive-optimizations and
 * -fno-strength-reduce are all inert.  -fno-gcse gets the line count to 76 but
 * 68 differing, and -O1 to 78/51.  There is no flag group here worth a rule.
 *
 * PROCESS NOTE.  Before finding `_TBL_L3` I had started renaming `.L3`/`.L13`
 * in the hand-written .s that exports them.  That broke the link -- three
 * overlay linker scripts reference the ROM names -- and it was unnecessary,
 * because docs/elevation.md already documents the alias mechanism under
 * "A short .LN extern can be captured by gcc's labels".  Grep the doc before
 * editing shared asm.
 */
