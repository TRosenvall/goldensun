/* Cluster OvlFunc_943_200b380..OvlFunc_943_200b380 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_7c7b9c/overlay.ld, so the ROM layout does
 * not move.
 *
 * Hides an actor: drops its draw priority, clears its sprite flags, clears
 * interactFlags, and sets bit 1 of flags (which shifts the sprite off-screen
 * by -0x140.0000 per include/actor.h).
 *
 * The slot number and the Actor pointer are both held across the calls, which
 * is why the ROM saves r5 and r6.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int slot, int mode);
extern void __Actor_SetSpriteFlags(Actor *a, int flags);

void OvlFunc_943_200b380(int slot)
{
    Actor *a;

    a = __MapActor_GetActor(slot);
    if (a == 0)
        return;
    __Func_8092b08(slot, 3);
    __Actor_SetSpriteFlags(a, 0);
    a->interactFlags = 0;
    a->flags |= 2;
}
