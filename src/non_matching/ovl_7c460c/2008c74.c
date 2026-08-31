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
 * CORPUS TEST -- SUGGESTIVE, NOT DECISIVE.  Zero of the 2987 generated .s
 * files contain the four-line sequence
 *
 *     mov rA, #K / mov rB, #0 / neg rA, rA / mov rC, #0
 *
 * but read that with care.  `neg` appears in only 241 of the 2987 files at all,
 * and two CONSECUTIVE `neg`s appear in 2 -- so a zero for a rarer four-line
 * pattern is a low base rate, not proof of impossibility.  An earlier version
 * of this note called it decisive and claimed the family was unreachable; that
 * was an overstatement, and the sibling park ovl_7cb2c0/200be34.c records the
 * detector bug that produced a matching false zero for a related shape.
 *
 * Treat this as: eleven functions share one residue, eleven spellings and three
 * flags have failed on it, and it is a poor use of screens -- not as proof.
 *
 * THE FAMILY -- 11 functions carry the idiom:
 *   OvlFunc_895_2008154      asm/overlays/rom_78dee8/ovl_30_c_c_a_a_a.s
 *   OvlFunc_916_20087e0      asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a.s
 *   OvlFunc_936_2008504      asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_c.s
 *   OvlFunc_939_2008c74      asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_c.s
 *   OvlFunc_941_2009760      asm/overlays/rom_7c5efc/ovl_30_c_c_c_c_c.s
 *   OvlFunc_950_200813c      asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a.s
 *   OvlFunc_952_2008674      asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s
 *   OvlFunc_952_2008af8      asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s
 *   OvlFunc_959_200b054      asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a.s
 *   OvlFunc_965_2009030      asm/overlays/rom_7ef4f4/ovl_30_a_c_a_c.s
 *   OvlFunc_966_20087c4      asm/overlays/rom_7f148c/ovl_30_c_c_c_a_a.s
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
 *
 * BATCH-156 FLAG -- THE CORPUS ZERO ABOVE IS PROBABLY A DETECTOR ARTIFACT.
 *
 * The "zero of the 2987 generated .s files" result should not be relied on. A
 * scan for a closely related shape over asm/**/*.s returned zero for a purely
 * mechanical reason: the .s files separate mnemonic from operands with a TAB,
 * so a regex written with a literal space matches NOTHING. Corrected, that
 * same scan returned 3846 sites across 555 functions.
 *
 * gcc 2.96's generated output was then checked directly and uses the tab
 * separator as well (`sub\tr0, r0, #8`), so the generated corpus carries the
 * identical hazard. Unless the original scan is known to have used `\s+`, its
 * zero says nothing about the `neg` family.
 *
 * This does not make the family reachable -- eleven spellings and three flags
 * still failed on it, and that evidence stands on its own. It removes the
 * corpus zero as SUPPORT for unreachability. Re-run the scan with `\s+`
 * before citing it again. See docs/elevation.md, "Corpus scans must use \s+".
 *
 * Separately: the statement-split lever found on Func_80b9a70 does not apply
 * here either. These are `f(0, 0, -8)` argument temporaries, dead at the call,
 * and gcc rematerialises those during argument fill.
 */
