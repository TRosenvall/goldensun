/* Overlay 888: detach slot 14's per-frame hook and park it at the origin.
 *
 * Split out of asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c.s; the neighbouring
 * parts stay as assembly and are listed around this one in
 * overlays/rom_7892c8/overlay.ld, so the ROM layout is unchanged.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);

/* Clears the update hook at +0x6C before moving the actor, so nothing runs on
 * it for the frame the move lands.
 */
void OvlFunc_888_200a660(void)
{
    Actor *actor = __MapActor_GetActor(0xe);

    actor->update = 0;
    __MapActor_SetPos(0xe, 0, 0);
}
