/* Cluster OvlFunc_900_2008094..OvlFunc_900_2008094 extracted from goldensun/asm/overlays/rom_797740/ovl_30_c_c_a_a_c_a.s.
 *
 * The .s held ONLY this function and no data -- confirmed with
 * tools/asmfacts.py.
 *
 * GreetSlot8. The slot-8 NPC's scripted greeting: turn toward slots 9 and 0xA
 * in turn forty frames apart, speak a line, re-form the followers, turn 8 and
 * 0 to face each other, drop 8 back to formation 1, and close with a second
 * line.
 *
 * WHICH CALLEES ARE DECLARED IS LOAD-BEARING, and this function needs both
 * halves of the rule at once. An implicitly declared callee returns int, so
 * gcc keeps r0 live across the call and fills the NEXT call's r0 last; a
 * declared one gets r0 first. Here:
 *
 *     r0 first  -> __ActorMessage, __Func_809259c        (declared)
 *     r0 last   -> __Func_809280c, __Func_80925cc,
 *                  __Func_8092848                        (left undeclared)
 *
 * Sixteen calls in forty-four instructions, and every one of them is pinned by
 * the calls around it -- which is why a function this size matched on the
 * first attempt where twenty-instruction ones have been costing whole rounds.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_809259c(int slot, int formation);

void OvlFunc_900_2008094(void)
{
    __CutsceneStart();
    __Func_809280c(8, 9, 0);
    __CutsceneWait(0x28);
    __Func_809280c(8, 0xa, 0);
    __CutsceneWait(0x28);
    __MessageID(0x138a);
    __ActorMessage(8, 0);
    __Func_809259c(9, 2);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    __Func_8092848(8, 0, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneEnd();
}
