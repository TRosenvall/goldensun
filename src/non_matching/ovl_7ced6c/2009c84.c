/* OvlFunc_946_2009c84 -- 0x02009c84,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s
 *
 * 77 lines against 77, and every one of the ELEVEN differing lines is the SAME
 * TWO REGISTERS SWAPPED.  Candidate at scratch/N9c84_best.c.
 *
 *      rom   asr r5, r3, #0x14 ... asr r6, r3, #0x14 ... cmp r6, #0x7
 *      ours  asr r6, r3, #0x14 ... asr r5, r3, #0x14 ... cmp r5, #0x7
 *
 * Two values are shifted out of actor fields at the top and both live to the
 * end.  The ROM puts the FIRST in r5 and the second in r6; we do the reverse.
 * Instruction count, instruction order and every operand other than those two
 * register numbers are exact.
 *
 * SOLVED ON THE WAY, and worth keeping: INLINING THE __MapActor_GetActor CALLS
 * took it from 16 differing to 11.  Written as `e = __MapActor_GetActor(8);
 * a = *(int *)(e + 8) >> 20;` the pointer becomes a fourth pseudo competing for
 * the allocator's attention and the third shifted value moves out of a scratch
 * register into r0 with the three-operand `asr`.  Written as
 * `a = *(int *)(__MapActor_GetActor(8) + 8) >> 20;` there is no pointer
 * variable at all and the two-operand `asr r3, #0x14` comes back.  A named
 * pointer that is dead by the next statement is not free.
 *
 * BLOCKER: ALLOCATION PRIORITY.  gcc's global allocator ranks by references per
 * unit of live range and hands out registers in REG_ALLOC_ORDER; with
 * -fcall-used-r4 the first callee-saved slot is r5, so whichever of the two
 * wins gets it.  Both have the same live range here -- computed at the top,
 * used at the last call -- and similar reference counts, so the two orderings
 * are a near tie that gcc breaks the other way.
 *
 * SCREENED AND UNCHANGED AT 11: all six declaration orders of the three locals
 * (tried on both the pointer and the inlined baseline); the reused third local
 * split into two so each use is short-lived; the decrement written `--a`,
 * `a = a - 1`, and moved to three different statement positions; and both stack
 * arguments named at one call site and then at both, which is the batch-148
 * six-argument lever and adds pseudos without changing the ranking.
 *
 * This is the class docs/elevation.md sizes under REG_ALLOC_ORDER: the note
 * there says the testable next step is rebuilding gcc-2.96 with the order
 * starting at r4 and re-screening. This function is a clean, small member to
 * test that on -- two pseudos, one swap, nothing else wrong.
 */
