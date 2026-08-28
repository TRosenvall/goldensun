/* Func_8028920 -- asm/rom_15000/rom_23178_a_a_a_a_c_c_c.s
 *
 * BLOCKER: POOL LOAD ORDER inside a constant computation -- 2 of 52, same length
 *
 *     rom  lsl r3,r6,#1 / add r3,r6 / ldr r2,=.L37403 / lsl r7,r3,#1
 *     ours lsl r3,r6,#1 / add r3,r6 / lsl r7,r3,#1    / ldr r2,=.L37403
 *
 * The ROM slots the table's pool load into the MIDDLE of the `m * 6` build;
 * gcc finishes the arithmetic first.  50 of 52 lines are exact, including both
 * signed-char table reads through the asm-label externs, the clamp pair and the
 * carried `k = m * 6`.
 *
 * MEASURED (all 52 lines unless noted):
 *   plain literals, k = m * 6                                  2
 *   the table base named in the DOMINATING entry block
 *     (`signed char *t = L37403;`)                56 of 52, ours 58 lines
 *   k split as `k = m * 3; k = k * 2;`                         9
 *   first use inlined as L37403[idx + m * 6], k assigned after 2
 *
 * The basic-block lever result is the informative one and it is a WARNING, not
 * a near miss: this function HAS labels, so the lever is available, and using
 * it made things six instructions worse.  Naming the table base kept it live
 * across the `_GetNumDjinn` call and cost a callee-saved register -- the same
 * over-naming trap recorded under "Naming one level too many costs a
 * callee-saved register".  The ROM re-loads the base at its point of use.
 *
 * Best C: scratch/w8920.c.
 */
