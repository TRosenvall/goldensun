/* Cluster OvlFunc_940_200816c..OvlFunc_940_200816c extracted from goldensun/asm/overlays/rom_7c5974/ovl_30_c_c_a_c_c_c_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7c5974/overlay.ld is
 * unchanged.
 *
 * Talking to a sanctum attendant: if the player is standing in a particular
 * facing range, open the sanctum menu; otherwise say one of two lines
 * depending on a save flag.
 *
 * THE FACING TEST IS A RANGE CHECK FOLDED INTO ONE COMPARE:
 *
 *     ldr r2, =0xffff5fff / ldrh r3, [r0, #6] / add r3, r2
 *     ldr r2, =0x3ffe / cmp r3, r2 / bhi .L0
 *
 * 0xffff5fff is -0xa001, so this is `(u32)(facing - 0xa001) <= 0x3ffe`, the
 * standard unsigned-wraparound spelling of `0xa001 <= facing <= 0xdfff`.
 * Writing the subtraction form directly reproduces it. Note r2 is loaded twice
 * with two different pooled constants and reused, which gcc does here without
 * help -- this is NOT the reassigned-mask situation from
 * src/non_matching/rom_15000/rom_1c154.c, because both operands are already
 * word-width and there is nothing for gcc to narrow.
 *
 * ONE TRANSPOSITION had to be fixed, on __ActorMessage:
 *
 *     rom    mov r0, #0x15 / mov r1, #0x0
 *     ours   mov r1, #0x0  / mov r0, #0x15
 *
 * Declaring the callee reverses the order gcc fills that call's own argument
 * registers. Same lever, same shape, as
 * src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_a_c_c_c_a.c in this batch -- both are
 * a single `mov r0` that wants to come first, and in both the fix is one extern
 * for the mismatching callee while everything else stays implicit.
 *
 * That is now the reliable reading of this shape: when exactly one call has its
 * r0 in the wrong position and the rest of the function is correct, declare
 * that callee before trying anything else.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_940_200816c(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    if (f - 0xa001 <= 0x3ffe) {
        __UI_Sanctum(0x15);
    } else if (__GetFlag(0x941) != 0) {
        __CutsceneStart();
        __MessageID(0x2507);
        __ActorMessage(0x15, 0);
        __CutsceneEnd();
    } else {
        __CutsceneStart();
        __MessageID(0x1bdc);
        __ActorMessage(0x15, 0);
        __CutsceneEnd();
    }
}
