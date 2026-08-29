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
 *
 * CORRECTED by src/non_matching/ovl_7b2078/200a68c.c.  The precondition stated
 * above -- that naming the shifted value works when a callee-saved register is
 * already committed -- is NOT sufficient.  OvlFunc_926_200a68c holds two
 * arguments across all eight of its calls, so it saves r5 and r6 before any of
 * this is decided, and naming its two shifted values STILL fails: 36 differing
 * and eight lines long, because the named locals want registers beyond the two
 * already committed and gcc spills.
 *
 * The narrower reading that survives both cases: the named constant has to fit
 * in registers the function was going to save anyway, with nothing else
 * competing for them.  One held value plus one named constant fits, which is
 * why OvlFunc_921_20087a4 matches.  Two plus two does not.  That is a property
 * of how many values are live at the call rather than something the C selects,
 * so it is a coincidence to check for, not a lever to reach for.
 *
 * READ THIS FIRST, AND BEFORE ANY OTHER FUNCTION IN THIS CLASS.
 *
 * This is the ARGUMENT PRECOMPUTE class, and it was diagnosed out of the
 * compiler sources long before this park was written.  See HANDOFF.md,
 * "Argument precompute: DIAGNOSED, and it is a compiler difference", and the
 * full derivation in src/non_matching/ovl_780898/2008dc0.c:
 *
 *     calls.c:805  precompute_register_parameters() copies any argument whose
 *                  rtx_cost > 2 into a pseudo BEFORE any hard register is
 *                  loaded, under SMALL_REGISTER_CLASSES && reg_parm_seen.
 *     arm.h:1061   SMALL_REGISTER_CLASSES is TARGET_THUMB -- always 1 here.
 *     arm.c:2042   In Thumb, ASHIFT costs 4, so a shifted constant is
 *                  "expensive" and gets hoisted; a cheap `mov` is emitted
 *                  afterwards and lands last.
 *
 * The ROM's compiler did not precompute.  That is the whole difference, and
 * HANDOFF.md states plainly that it is NOT FIXABLE FROM C -- eight source
 * spellings and eight flags are byte-identical to the default.
 *
 * The "callee-saved register already committed" theory recorded elsewhere in
 * these notes was an invented explanation for this same behaviour, arrived at
 * without reading the existing diagnosis.  It is not a lever and should not be
 * treated as one.  Anything above that reasons about which registers are
 * committed is describing a symptom of the hoist, not its cause.
 *
 * The predictive rule from HANDOFF.md, which is the useful part: a call
 * misorders when its argument list mixes cheap constants with expensive values
 * and a cheap one is not last.  A call whose arguments are ALL cheap constants
 * matches.  Check that before screening, not after.
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
