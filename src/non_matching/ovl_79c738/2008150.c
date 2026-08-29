/* OvlFunc_909_2008150 -- NON-MATCHING.
 * Blocker class: ARGUMENT INTERLEAVE, ONE-EXPENSIVE-VALUE VARIANT.
 * 4 of 36, same length, and all four are two instructions swapped at two calls.
 *
 *     rom    mov r1,#0x81 / mov r2,#0 / mov r0,#0xe / lsl r1,#1
 *     ours   mov r1,#0x81 / mov r2,#0 / lsl r1,#1   / mov r0,#0xe
 *
 * WHY THIS IS FILED SEPARATELY FROM ARGUMENT PRECOMPUTE. HANDOFF.md's rule is
 * "cheap constants mixed with TWO OR MORE expensive values, and a cheap one is
 * not last". Every call here has exactly ONE expensive value -- a single
 * shifted constant. The rule does not cover this, and the mechanism it names
 * (calls.c:805 hoisting expensive arguments ahead of the register loads) does
 * not obviously explain a swap of two cheap-and-one-expensive.
 *
 * This function was invisible until the census filter was tightened: the loose
 * version flagged it as precompute and it was never screened. It is 4 of 36,
 * which is much closer than the class it was filed under would suggest, and
 * there may be more like it among the ~130 functions the loose filter was
 * over-counting.
 *
 * Tried, all identical at 4 differing:
 *   - the shift inline in the call
 *   - the shifted value in a named local assigned immediately before the call
 *   - the slot id 0xe in a named local, used at all nine call sites
 *
 * The last is worth recording because 0xe appears in nine calls here, which is
 * the strongest case yet for gcc keeping it in a register -- and it still
 * rematerialises, exactly as ovl_7f148c/200810c.c found with five uses.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2008150(void)
{
    __CutsceneStart();
    __MapActor_Emote(0xe, 0x81 << 1, 0);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x28);
    __MessageID(0x1764);
    __Func_8093040(0xe, 0, 0x14);
    __Func_809280c(0xe, 0, 0);
    __CutsceneWait(0x14);
    __Func_8093040(0xe, 0, 0xa);
    __Func_8092adc(0xe, 0xb0 << 8, 0xa);
    __CutsceneEnd();
}
