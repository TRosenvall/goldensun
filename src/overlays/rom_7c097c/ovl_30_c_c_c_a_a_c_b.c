/* Overlay 936: set the display-offset flag on the player actor.
 *
 * Split out of asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c.s; the neighbouring
 * parts stay as assembly and are listed around this one in
 * overlays/rom_7c097c/overlay.ld, so the ROM layout is unchanged.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

/* Bit 1 of +0x23 shifts the sprite by -0x140.0000 (see actor.h). Setting bit 0
 * here is a different flag in the same byte; what it does is not established.
 */
void OvlFunc_936_20095e0(void)
{
    Actor *actor = __MapActor_GetActor(0);

    actor->flags |= 1;
}
