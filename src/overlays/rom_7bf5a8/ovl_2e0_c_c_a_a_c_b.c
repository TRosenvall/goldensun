/* Cluster OvlFunc_935_2008554..OvlFunc_935_2008554 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c.s.
 *
 * Total .text for this TU = 76 bytes (= 0x4c).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_a.o and asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld.
 */
#include "actor.h"

void OvlFunc_935_2008554(void)
{
  struct Actor *actor;
  struct Actor *actor2;
  int s1;
  int s2;

  actor = (struct Actor *) __MapActor_GetActor(0x11);
  s1 = 0x17;
  s2 = 0x22;
  __Func_8010704(0x1a, 0x1e, 1, 1, s1, s2);
  if (actor != 0)
  {
    actor2 = (struct Actor *) __MapActor_GetActor(0x11);
    actor2->interactFlag = 0;
    actor->flags = 1;
  }
  __SetFlag(0x201);
}
