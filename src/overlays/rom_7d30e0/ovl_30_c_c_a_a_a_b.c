/* Cluster OvlFunc_948_2009984..OvlFunc_948_2009984 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_a.s.
 *
 * Total .text for this TU computed at build time from expected/.../.o.
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_a_a.o and asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_a_c.o in
 * goldensun/overlays/rom_7d30e0/overlay.ld.
 */
#include "actor.h"

void OvlFunc_948_2009984(void)
{
  struct Actor *actor;
  struct Actor *actor2;
  int s1;
  int s2;

  actor = (struct Actor *) __MapActor_GetActor(0xd);
  s1 = 0x28;
  s2 = 0x37;
  __Func_8010704(0x28, 0x36, 1, 1, s1, s2);
  if (actor != 0)
  {
    actor2 = (struct Actor *) __MapActor_GetActor(0xd);
    actor2->interactFlag = 0;
    actor->flags = 2;
  }
  __SetFlag(0x200);
}
