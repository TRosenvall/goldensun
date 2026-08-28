/* OvlFunc_966_200810c -- NON-MATCHING.  Blocker class: INTERLEAVE.
 *
 * 27 lines against the ROM's 27, 5 differing, in two places and both the same
 * shape -- the ROM puts the FIRST argument in the middle of another
 * argument's construction and gcc puts it after:
 *
 *     rom   mov r1,#0x80 / mov r2,#0x80 / mov r0,#0x12 / lsl r1,#9 / lsl r2,#8
 *     ours  mov r1,#0x80 / mov r2,#0x80 / lsl r1,#9   / lsl r2,#8 / mov r0,#0x12
 *
 *     rom   mov r1,#0x10 / mov r0,#0x12 / neg r1,r1
 *     ours  mov r1,#0x10 / neg r1,r1    / mov r0,#0x12
 *
 * Same family as src/non_matching/ovl_794ac0/2008428.c, and the note there
 * carries the full analysis.  This one is worth keeping separately because it
 * TESTS that note's conclusion and confirms it.
 *
 * That note ends by narrowing the lever: naming the constant works only when a
 * callee-saved register is already committed for another reason.  This
 * function looked like the case that could satisfy it from the C side -- the
 * slot id 0x12 is passed to FIVE separate calls, so a named local for it has
 * every reason to be kept live across them.
 *
 * It is not.  `slot = 0x12` named once and used five times is byte-identical
 * to the literal: still 5 differing, still `push {lr}` with no register saved.
 * gcc-2.96 rematerialises a small constant at each call site rather than
 * holding it, however many uses it has.
 *
 * So the precondition really is "a callee-saved register committed by
 * something the ROM also commits it for" -- a pointer or a loaded value that
 * survives a call.  Repeated use of a cheap constant does not create it, and
 * that closes the one remaining idea for reaching this class from the C.
 *
 * Tried: the literal inline (5 differing); `slot = 0x12` named and used five
 * times (5 differing, identical output).
 */
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_966_200810c(void)
{
    __SetFlag(0x9bb);
    __MessageID(0x28b8);
    __ActorMessage(0x12, 0);
    __MapActor_SetSpeed(0x12, 0x80 << 9, 0x80 << 8);
    __Func_8092304(0x12, -0x10, 0);
    __Func_8092adc(0x12, 0, 0);
    __CutsceneWait(0xa);
}
