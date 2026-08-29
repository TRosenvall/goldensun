/* OvlFunc_974_200829c -- 0x0200829c, asm/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_a.s
 *
 * NOT TRANSCRIBED, and deliberately so.  Parked on a pre-screen rather than on a
 * failed match, which is worth recording because the profile is a trap.
 *
 * On every candidate metric this is the most attractive function left in the
 * dense queue: 588 instructions, 196 calls, THREE distinct callees
 * (__Func_801776c, __GiveItemTo, __CalcStats), zero memory operations, zero
 * conditional branches, zero shifted constants, reuse 0.  It is a third sibling
 * of OvlFunc_974_2008bb8 and OvlFunc_974_2008f14, both elevated, and
 * draft_script.py produces its 196 calls in one go.
 *
 * It cannot match.  At instruction 385 it jumps over its own literal pool:
 *
 *          b .L6a0
 *          .pool_aligned
 *      .L6a0:
 *          mov r0, #2
 *
 * old_agbcc emits a function's pool at .func_end and never in the middle, so
 * that `b` is unreachable -- measured as ZERO occurrences across all elevated
 * translation units, with the translation-unit hypothesis separately refuted.
 *
 * tools/poolblocked.py was written from this and reports 312 of the 2,239
 * remaining functions in the same position, 13.9%.  Run it before drafting
 * anything large; the transcription here would have been 196 calls for nothing.
 */
