/* Overlay 942: hide an actor and mark it non-interactive.
 *
 * Split out of asm/overlays/rom_7c6bac/ovl_30_c_c_c.s; the neighbouring parts
 * stay as assembly and are listed around this one in
 * overlays/rom_7c6bac/overlay.ld, so the ROM layout is unchanged.
 *
 * The slot is resolved TWICE -- once into a local that survives the calls, and
 * again as the argument to __Actor_SetSpriteFlags. Caching it and passing the
 * local instead is one instruction shorter than the ROM; the repetition is in
 * the original.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(Actor *actor, int flags);
extern void __Func_8092b08(int slot, int arg);

void OvlFunc_942_2008b68(int slot)
{
    Actor *actor = __MapActor_GetActor(slot);

    __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
    __Func_8092b08(slot, 3);
    actor->interactFlag = 0;
    actor->flags |= 2;
}
