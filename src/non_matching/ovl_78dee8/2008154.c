/* OvlFunc_895_2008154 -- NON-MATCHING.  Blocker class: ARGUMENT PRECOMPUTE.
 *
 * Diagnosed already.  See HANDOFF.md, "Argument precompute: DIAGNOSED, and it
 * is a compiler difference", and the derivation in
 * src/non_matching/ovl_780898/2008dc0.c.  Nothing here is new; this note
 * exists to record that the class's PREDICTIVE RULE was used in advance and
 * held, rather than to re-derive anything.
 *
 * 64 lines against the ROM's 64, 5 differing, all at one call:
 *
 *     rom   mov r0, #0x0 / ldr r1, =0x9999 / ldr r2, =0x4ccc
 *     ours  ldr r1, =0x9999 / ldr r2, =0x4ccc / mov r0, #0x0
 *
 * The rule in HANDOFF.md is: a call misorders when its argument list mixes
 * cheap constants with expensive values and a cheap one is not last.
 * `__MapActor_SetSpeed(0, 0x9999, 0x4ccc)` is exactly that shape -- a cheap
 * zero FIRST, two pool loads after it -- so this was expected to misorder
 * before it was screened, and it does.  calls.c:805 hoists the pool loads
 * ahead of the register loads because their rtx_cost exceeds 2; the cheap
 * `mov` is emitted afterwards and lands last.  The ROM's compiler did not
 * precompute.
 *
 * Not fixable from C, so nothing else was tried.  The three CopyMapTiles calls
 * above it, whose fifth and sixth arguments go on the stack, are byte-exact
 * from two named locals -- that part of the function is finished.
 *
 * WHY THE .s WAS SPLIT.  This function shared
 * asm/overlays/rom_78dee8/ovl_30_c_c_a_a_a.s with OvlFunc_895_200807c and
 * OvlFunc_895_20080ec, both of which DO match.  One unreachable function was
 * holding two reachable ones as assembly, so the .s was split with
 * tools/split_s.py: the two matches now live in ovl_30_c_c_a_a_a_a.c and this
 * one stays as ovl_30_c_c_a_a_a_b.s.  The split was verified byte-neutral
 * (make compare green) BEFORE any .c was written, which is what that tool's
 * own instructions ask for.
 */
