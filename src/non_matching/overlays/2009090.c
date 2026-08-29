/* OvlFunc_966_2009090 -- 0x02009090, asm/overlays/rom_7f148c/ovl_30_c_c_c_a_a.s
 *
 * 111 ROM lines against 110 of ours, 89 differing -- but the whole of that is
 * ONE decision, and the body is otherwise transcribed correctly.
 *
 * A flat 37-call cutscene script.  Four of the calls pass 0x100, which needs a
 * two-instruction build (mov #0x80 / lsl #1).  gcc hoists that build out, keeps
 * the value in r5 across the calls, and emits `mov r1, r5` at each of the four
 * sites.  The ROM rebuilds it in place every time and never touches r5.
 *
 * That accounts for the length exactly: four sites saving one instruction each,
 * minus the two-instruction hoist, is -2, and the extra `push {r5}` the reuse
 * forces brings it to -1.  Everything after the hoist is shifted by one, which
 * is where the 89 comes from -- it is one blocker, not eighty-nine.
 *
 * This is the constant_reuse class; see constant_reuse.c in this directory.
 *
 * TRIED, none of which moved it: GCSE, CSE, ALIAS, SCHED2, O1.  No flag group
 * in the tree reaches the decision, and the four uses are the same folded
 * integer, so no source spelling separates them either -- `0x80 << 1` and
 * `0x100` are the same constant by the time the back end sees them.
 */
