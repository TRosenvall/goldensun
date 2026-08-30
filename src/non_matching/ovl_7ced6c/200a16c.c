/* OvlFunc_946_200a16c -- 0x0200a16c,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_a.s
 *
 * 72 of 72 lines, THIRTEEN differing, and every one of the thirteen is the same
 * two values in swapped registers.  Candidate at scratch/Na16c_best.c.
 *
 *      rom   asr r6, r3, #0x14 ... asr r5, r3, #0x14 ... cmp r6, #0x19
 *      ours  asr r5, r3, #0x14 ... asr r6, r3, #0x14 ... cmp r5, #0x19
 *
 * Identical residue to src/non_matching/ovl_7ced6c/2009c84.c, two functions
 * over in the same overlay: two `>> 20` values live to the end of the function,
 * similar reference counts, and which of them wins r5 is a near tie that gcc
 * breaks the other way.  Thirteen spellings are now screened across the two --
 * all six declaration orders, the reused local split, the decrement in three
 * positions and two forms, the stack arguments named at one site and at both,
 * and here the fetches written through a named pointer instead of inlined.
 * None of them moves it.
 *
 * WORTH NOTING, because it rules out the obvious rule: the two functions
 * disagree about WHICH value the ROM favours.  On 2009c84 the ROM puts the
 * FIRST-computed value in r5; here it puts the first-computed in r6.  In both
 * cases we produce the opposite of the ROM.  So there is no "first one gets r5"
 * to encode -- it is the allocator's density ranking, and the source has no
 * handle on it.
 *
 * NOT re-spent here.  This is the second data point for the same coin flip and
 * a clean two-pseudo test case for the REG_ALLOC_ORDER hypothesis that
 * docs/elevation.md says needs a rebuilt compiler to settle.  Retry both
 * together when it is.
 *
 * Everything else screens exact, including the shared call block at .L21ba that
 * four arms branch into -- which is NOT the shared-call-tail class of
 * src/non_matching/ovl_common/4cc.c: here gcc reproduces the shared call and the
 * line count matches, because the arms set two argument registers rather than
 * one value gcc can hoist above the compare.
 */
