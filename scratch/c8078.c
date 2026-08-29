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
