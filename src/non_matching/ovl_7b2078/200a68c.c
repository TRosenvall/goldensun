/* OvlFunc_926_200a68c -- NON-MATCHING.  Blocker class: INTERLEAVE.
 *
 * 30 lines against the ROM's 30, THREE differing, all in one argument block:
 *
 *     rom   mov r1,#0xa0 / mov r2,#0xa0 / mov r0,#0x0 / lsl r1,#10 / lsl r2,#9
 *     ours  mov r1,#0xa0 / mov r2,#0xa0 / lsl r1,#10 / lsl r2,#9   / mov r0,#0
 *
 * The ROM sinks BOTH shifts past the third argument; gcc finishes both before
 * starting it.  Everything else, including the two held arguments and all
 * seven other calls, is byte-identical.
 *
 * THIS FUNCTION FALSIFIES THE PRECONDITION RECORDED IN
 * src/non_matching/ovl_794ac0/2008428.c AND ovl_7f148c/200810c.c.
 *
 * Those notes concluded that naming the shifted value works only when a
 * callee-saved register is already committed for another reason -- the theory
 * being that OvlFunc_921_20087a4 matches that way because it already holds an
 * actor pointer and a facing value across its calls.
 *
 * Here the precondition is SATISFIED and the lever still fails.  This function
 * takes two arguments and holds both across all eight calls, so it opens with
 * `push {r5, r6, lr}` before any of this is decided.  Naming the two shifted
 * values anyway gives 36 differing and a stream EIGHT lines long, because the
 * named locals want registers BEYOND the two already committed and gcc spills
 * to make room.
 *
 * So the corrected reading is narrower than "a register is already
 * committed": it is that the named constant must fit in registers the
 * function was going to save ANYWAY, with nothing else competing for them.
 * One held value plus one named constant fits.  Two held values plus two
 * named constants does not, and the attempt costs more than it saves.
 *
 * That is a property of how many values are live at the call, which the C
 * chooses only indirectly.  It is not a lever so much as a coincidence that
 * can be checked for, and the check is: count what the ROM's prologue saves,
 * and count what naming would add.
 *
 * Tried:
 *   - shifts inline:                                    3 differing
 *   - both shifted values named at the top of the body: 36 differing, +8 lines
 *   - both named after the first call, immediately
 *     before the call that uses them:                    3 differing
 *   - the folded constants 0x28000 and 0x14000:          3 differing
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
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_926_200a68c(int a, int b)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0xa0 << 10, 0xa0 << 9);
    __Func_809228c(0, a, b);
    __MapActor_Jump(0, 4, 0);
    __MapActor_SetAnim(0, 7);
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 6);
    __CutsceneEnd();
}
