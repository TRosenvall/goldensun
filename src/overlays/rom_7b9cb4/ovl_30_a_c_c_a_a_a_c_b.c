/* Cluster OvlFunc_932_200850c..OvlFunc_932_200850c extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c.s.
 *
 * Total .text for this TU = 80 bytes (= 0x50).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_a.o and asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c.o in
 * goldensun/overlays/rom_7b9cb4/overlay.ld.
 */
#include "actor.h"

void OvlFunc_932_200850c(void)
{
  struct Actor *actor;

  actor = (struct Actor *) __MapActor_GetActor(9);
  __Func_8010704(0, 0, 1, 1, 0x1a, 0x1a);
  if (actor != 0)
  {
    __Actor_SetSpriteFlags(actor, 0);
    actor->pos.y += -0x200000;
    actor->flags = 2;
  }
  __SetFlag(0x200);
}
