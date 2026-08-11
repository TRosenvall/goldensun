/* Cluster OvlFunc_911_2008230..OvlFunc_911_2008230 extracted from goldensun/asm/overlays/rom_79e5c0/ovl_30_c_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_79e5c0/ovl_30_c_a_a_a_a.o and asm/overlays/rom_79e5c0/ovl_30_c_a_a_a_c.o in
 * goldensun/overlays/rom_79e5c0/overlay.ld.
 *
 * Another sanctum attendant: stand in the right facing arc and the menu opens,
 * otherwise you get a line of dialogue. Same shape as
 * src/overlays/rom_7c5974/ovl_30_c_c_a_c_c_c_c_a.c in a different overlay, with
 * different ids and a different fallback call.
 *
 * ONE THING DIFFERS FROM THAT TWIN AND IT IS LOAD-BEARING: the facing is read
 * BEFORE __CutsceneStart and the subtraction happens after --
 *
 *     ldrh r5, [r0, #6] / bl __CutsceneStart / ldr r3, =0xffff5fff / add r5, r3
 *
 * so the value is live across the call and lands in a callee-saved register.
 * Reading it after the call, which is how the twin is written, produces a
 * different prologue. The read has to be its own statement before the call.
 *
 * The range test is the folded unsigned-wraparound form,
 * `(u32)(facing - 0xa001) <= 0x3ffe`, with no cast and therefore no shift --
 * see src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_a.c for the variant that DOES
 * narrow and emits `lsl #16` against a pre-shifted constant.
 *
 * No declarations needed; every call here wants r0 filled last.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_911_2008230(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    __CutsceneStart();
    if (f - 0xa001 <= 0x3ffe) {
        __UI_Sanctum(0x10);
    } else {
        __MessageID(0x16b3);
        __Func_8093054(0x10, 0);
    }
    __CutsceneEnd();
}
