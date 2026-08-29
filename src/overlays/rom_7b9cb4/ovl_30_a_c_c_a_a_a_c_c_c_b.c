/* Cluster OvlFunc_932_20085ac..OvlFunc_932_20085ac extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c.s.
 *
 * Total .text for this TU = 80 bytes (= 0x50).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_a.o and asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c.o in
 * goldensun/overlays/rom_7b9cb4/overlay.ld.
 */
#include "actor.h"

void OvlFunc_932_20085ac(void)
{
  struct Actor *actor;
  int s1;
  int s2;

  actor = (struct Actor *) __MapActor_GetActor(9);
  s1 = 0x2b;
  s2 = 0x29;
  __Func_8010704(0x2d, 0x29, 1, 1, s1, s2);
  if (actor != 0)
  {
    __Actor_SetSpriteFlags(actor, 0);
    actor->pos.y += 0xffe00000;
    actor->flags = 2;
  }
  __SetFlag(0x200);
}
