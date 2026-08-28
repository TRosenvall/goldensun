/* OvlFunc_966_2008078  [ovl_7f148c]
 *
 * Source asm: goldensun/asm/overlays/rom_7f148c/ovl_30_c_c_a_c_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function and
 * no data.
 *
 * Twenty-seven instructions against twenty-seven, and the first fourteen are
 * identical. Blocker: ARG-INTERLEAVE, on __Func_8092adc:
 *
 *     rom    mov r1, #0xc0 / mov r0, r6  / lsl r1, #0x8 / mov r2, #0x0
 *     ours   mov r1, #0xc0 / lsl r1, #0x8 / mov r2, #0x0 / mov r0, r6
 *
 * The ROM puts r0 BETWEEN the two halves of building r1. See
 * src/non_matching/ovl_794ac0/2008428.c for the class and for why the
 * declaration levers do not reach it -- they move r0 to the front or the back
 * of an argument block, not into the middle of another argument's
 * construction. Both were tried here as well and neither moves it.
 *
 * SECOND FUNCTION ON THE SAME CALLEE. The parked OvlFunc_899_2008428 calls
 * __Func_8092adc(0xf, 0x80 << 8, 0) and this one calls
 * __Func_8092adc(slot, 0xc0 << 8, 0). Both interleave, and in the first the r0
 * operand is an IMMEDIATE while here it is a REGISTER MOVE, so the shape does
 * not depend on what r0 is being loaded from.
 *
 * The common factor is the second argument being built with `mov` + `lsl`.
 * That gives gcc a two-instruction sequence to schedule around, and the ROM
 * fills r0 in the gap while gcc fills it after. Any call of the form
 * `f(x, N << k, ...)` is a candidate for this, which is now a filter in
 * tools/pick_candidates.py rather than something to rediscover per function.
 *
 * Everything else here is right and needed no lever, including holding
 * 0x10000 in a callee-saved register across a call and reusing it for both
 * stores -- that is a value gcc COMPUTED, so it reuses it exactly as the ROM
 * does, unlike the constant-CSE cases where the ROM rebuilds.
 *
 * BATCH 93 -- ONE OF THE THREE IS GONE, and the lever is batch 92's. This file
 * called __Func_8092adc with NO declaration in scope. Adding
 * `extern void __Func_8092adc(int, int, int);` moves `mov r2, #0` into place
 * and takes the count from 3 differing to 2:
 *
 *     rom     mov r1, #0xc0 / mov r0, r6 / lsl r1, #8 / mov r2, #0
 *     before  mov r1, #0xc0 / lsl r1, #8 / mov r2, #0 / mov r0, r6
 *     after   mov r1, #0xc0 / lsl r1, #8 / mov r0, r6 / mov r2, #0
 *
 * Batch 92 found the rule going the other way -- REMOVING a prototype pushed
 * r0 to the end for Func_80a47b4 -- and this is the same rule read forwards.
 * The prototype is in the source below.
 *
 * What is left is the genuine interleave: the ROM slots `mov r0, r6` BETWEEN
 * `mov r1, #0xc0` and its `lsl r1, #8`, splitting a shifted constant's two
 * halves around another argument. That is the blocker this file was filed
 * under and it is untouched.
 *
 * RETRIED with the lever recovered from OvlFunc_932_200a9dc -- name the OTHER
 * arguments and leave the one you want moved as a bare expression.  Naming the
 * shifted argument alone, and naming it together with the zero, both leave the
 * count at 2; naming them further up costs an instruction and 19 positions.
 *
 * That is the documented precondition holding rather than the lever failing:
 * 200a9dc had a conditional branch before its site to name in, and this function
 * is straight-line with no branch anywhere, so there is no block that dominates
 * the call without being the call's own.  Worth having screened, because the
 * "name the others" form had not been tried here and it is what made the
 * difference on the guarded case.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern unsigned char ActorCmd_ARRAY_966__02009638[];
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_966_2008078(int slot)
{
    Actor *a;
    fx32 v;

    a = __MapActor_GetActor(slot);
    v = 0x80 << 9;
    a->rotX = v;
    a = __MapActor_GetActor(slot);
    a->rotY = v;
    __MessageID(0x26af);
    __ActorMessage(slot, 0);
    __Func_8092adc(slot, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(slot, ActorCmd_ARRAY_966__02009638);
}
