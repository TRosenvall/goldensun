/* OvlFunc_939_2008c74 -- asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_c.s
 *
 * BLOCKER: THE `neg` INTERLEAVE -- a two-line transposition, family of >=2
 *
 * 2 of 53, same length.  51 lines exact.  The entire difference:
 *
 *     rom  mov r2,#8 / mov r1,#0 / neg r2,r2 / mov r0,#0
 *     ours mov r2,#8 / neg r2,r2 / mov r1,#0 / mov r0,#0
 *
 * gcc completes the two-instruction build of -8 before touching the other
 * arguments; the ROM slots the `mov r1,#0` between the `mov` and the `neg`.
 *
 * SAME IDIOM, SAME 2-LINE RESIDUE, in OvlFunc_936_2008504 (different callee --
 * __Func_809228c rather than __Func_80922c4 -- identical instruction sequence).
 * Both are `f(0, 0, -8)`.  Solve it once and two functions land; see
 * src/non_matching/ovl_7c097c/2008504.c.
 *
 * MEASURED (all 53 lines, all 2 differing at position 18):
 *   plain literal -8                                        2
 *   `int m = -8;` immediately before the call                2
 *   `int m = -8;` at the TOP of the function                43 (54 lines, worse)
 *   `int v = 8;` and the argument spelled `-v`               2
 *   `int z = 0;` shared by arguments 1 and 2                 2
 *   argument spelled `0 - 8`                                 2
 *   third parameter declared `short`                         2
 *   prototype removed entirely (the batch-117 lever)         2
 *   -fno-schedule-insns                                      2
 *   -fno-peephole                                            2
 *   -fno-schedule-insns2                                    23 (worse)
 *
 * The named-local-at-the-top result is the informative one: it is the only
 * spelling that moves the count and it moves it the wrong way, which is the
 * usual sign that 2 is a floor rather than a near miss.  Note also that
 * -fno-schedule-insns does NOT touch it, so whatever reorders the pair is not
 * the pre-reload scheduler.
 */
