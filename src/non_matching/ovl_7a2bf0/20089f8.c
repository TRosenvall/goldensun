/* OvlFunc_915_20089f8 -- 0x020089f8,
 * asm/overlays/rom_7a2bf0/ovl_30_c_c_a_a.s
 *
 * 74 of 74 lines, TWO differing, and they are one swap inside one call's
 * argument setup.  Candidate at scratch/N89f8_best.c.
 *
 *      rom   mov r3, #1 / mov r5, #0 / mov r0, #2 / str r5, [sp, #4]
 *      ours  mov r3, #1 / mov r0, #2 / mov r5, #0 / str r5, [sp, #4]
 *
 * SOLVED: everything else, and by template.  This is the same frame as
 * OvlFunc_927_2009454 and OvlFunc_927_20099b8 (both elevated) -- a 24-byte
 * struct filled by ..._2008474 and passed BY VALUE to ..._2008608, which is
 * what the `ldmia r2!, {r0,r1} / stmia r3!, {r0,r1}` block copy means.  The
 * batch-149 stack-argument rules carry over exactly: the (0xb, 0x10) pair is
 * built fresh into two registers so it gets its own pair of locals, and the
 * trailing zero is held in r5 across the preceding __Func_8010704 call so its
 * assignment is placed to span that call.  Both are right here; the zero IS in
 * r5, which is the part that usually goes wrong.
 *
 * BLOCKER: the REMATERIALISATION POINT of that zero.  gcc emits it one slot
 * later than the ROM -- after `mov r0, #2` rather than before it.  The value,
 * the register and the store position are all correct; only where reload drops
 * the `mov` differs, and that is chosen after allocation.
 *
 * SCREENED, all still 2: the assignment moved before the pair, after the pair,
 * before the actor store, and before the whole arm; the declaration reordered
 * so the zero is the first local; the same zero shared with the trailing
 * __Actor_SetSpriteFlags argument; and the per-declaration prototype sweep over
 * every callee (tools/protolever.py).  WORSE: both pair members hoisted
 * together above the __Func_8010704 call (10 differing), the pair written
 * adjacent immediately before the call (8), and the zero left as a literal,
 * which drops it out of r5 entirely (6).
 *
 * Note the contrast with OvlFunc_927_2009454, which matched: there the ROM
 * materialises BOTH pair members adjacently at the top of the setup and stores
 * the first at once; here it stores the first at once but materialises the zero
 * late, between the last register argument and r0.  Same rule, different
 * schedule, and no spelling reaches the difference.
 */
