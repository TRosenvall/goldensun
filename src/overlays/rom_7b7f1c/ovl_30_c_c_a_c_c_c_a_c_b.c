/* Cluster OvlFunc_930_20088e0..OvlFunc_930_20088e0 extracted from goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_a.o and asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_c.o in
 * goldensun/overlays/rom_7b7f1c/overlay.ld.
 *
 * A sanctum attendant, sixth of the shape. Identical to
 * src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_b.c apart from the actor id
 * and the message id, so read that header for the three spellings of the facing
 * range check and for why the facing needs its own local here (it is read
 * before __CutsceneStart and so is live across the call).
 *
 * Written by copying the twin and changing constants, which is worth a note
 * because the copy was WRONG on the first screen: the actor id appears twice --
 * once in __UI_Sanctum and once in __ActorMessage -- and only one of them got
 * changed. One differing instruction, `mov r0, #0xf` against `mov r0, #0xd`.
 *
 * That is exactly the class of bug that six parked functions were sitting on,
 * and it was caught here in one screen by the same check that finds them: on a
 * short diff, look at whether the differing operand is a VALUE before assuming
 * register allocation. Copying a twin is the cheapest way to produce one of
 * these, so check every constant that appears more than once.
 */
#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_930_20088e0(void)
{
    Actor *a;
    u32 f;

    a = __MapActor_GetActor(0);
    f = a->facing;
    __CutsceneStart();
    if (f - 0xa001 <= 0x3ffe) {
        __UI_Sanctum(0xf);
    } else {
        __MessageID(0x1a1e);
        __ActorMessage(0xf, 0);
    }
    __CutsceneEnd();
}
