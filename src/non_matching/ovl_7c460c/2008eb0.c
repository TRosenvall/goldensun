/* OvlFunc_939_2008eb0 -- 0x02008eb0, asm/overlays/rom_7c460c/ovl_314_a_c_c_c_c.s
 *
 * 94 of 94 lines, EIGHT differing, all from one decision.
 * Candidate: scratch/V8eb0.c.
 *
 * The body is otherwise complete: the gState halfword guard, four calls whose
 * arguments interleave, the message and flag ids, and the three six-argument
 * __Func_8010704 calls with 0xb held across them in r5 all reproduce untouched.
 *
 * BLOCKER: constant reuse.  __MapActor_Emote is called twice with the same
 * second argument, 0x80 << 1.  gcc builds it ONCE, before __CutsceneStart, keeps
 * it in r5, and passes `mov r1, r5` at both sites; the ROM rebuilds
 * `mov r1, #0x80 / lsl r1, #1` at each.  There is no added push -- the function
 * already saves r5 for the 0xb -- so the usual prologue tell is absent here and
 * only the body shows it.
 *
 * SECOND COUNTEREXAMPLE TO THE TWO-REMEDY RULE.  docs/elevation.md records that
 * this tell yields either to CSE_CFLAGS or to separate named locals.  As with
 * OvlFunc_881_2009c08, BOTH fail, and so does every other flag group:
 *
 *   CSE                 8    separate named locals (e1, e2)   8
 *   GCSE                8    separate locals + CSE            8
 *   ALIAS               8    STRENGTH                         8
 *   SCHED2             44    O1                              44
 *
 * Two independent counterexamples now, so the rule should be read as "try both,
 * expect neither" rather than as a fix with two spellings.
 *
 * Worth noting for selection: pool.py called this in advance with reuse = 1, and
 * the reuse column pointed at exactly the right constant.  The column predicts
 * the blocker reliably; it is the REMEDY that does not exist.
 */
