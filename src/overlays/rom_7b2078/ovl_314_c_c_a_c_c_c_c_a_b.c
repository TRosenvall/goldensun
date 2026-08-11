/* Cluster OvlFunc_926_200a508..OvlFunc_926_200a508 extracted from goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_a.o and asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_c.o in
 * goldensun/overlays/rom_7b2078/overlay.ld.
 *
 * A sanctum attendant. Fifth of this shape and the third distinct variant of
 * the facing test, so the family is worth stating in one place:
 *
 *   UNSHIFTED, no cast   `f - 0xa001 <= 0x3ffe` on a u32. No shift emitted.
 *                        This function, and rom_7c5974 / rom_79e5c0.
 *   SHIFTED, u16 cast    `(u16)(f - 0x6001) <= 0x7ffe`. gcc shifts ONCE and
 *                        compares against a PRE-SHIFTED constant rather than
 *                        narrowing with lsl/lsr. See rom_7c097c.
 *   SHIFTED, additive    `(u16)(f + 0x5fff) <= 0x3ffe`. Same as above with the
 *                        offset written as an addition. See rom_7c3044, in this
 *                        batch.
 *
 * The ROM shows which one the original used: a pre-shifted constant in the
 * comparison means a narrowing cast, and no shift at all means the test was
 * done at full width.
 *
 * As in src/overlays/rom_79e5c0/ovl_30_c_a_a_a_b.c, the facing is read BEFORE
 * __CutsceneStart and the subtraction happens after, so the value is live
 * across the call. One declaration, on __ActorMessage.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_926_200a508(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    __CutsceneStart();
    if (f - 0xa001 <= 0x3ffe) {
        __UI_Sanctum(0xd);
    } else {
        __MessageID(0x1a1c);
        __ActorMessage(0xd, 0);
    }
    __CutsceneEnd();
}
