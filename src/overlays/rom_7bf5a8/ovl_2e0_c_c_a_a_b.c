/* Cluster OvlFunc_935_200850c..OvlFunc_935_200850c extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a.s.
 *
 * Total .text for this TU = 72 bytes (= 0x48).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o and asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld.
 */
#include "actor.h"

void OvlFunc_935_200850c(void)
{
  struct Actor *actor1;
  struct Actor *actor2;
  int s1;
  int s2;

  actor1 = (struct Actor *) __MapActor_GetActor(0x10);
  s1 = 0x17;
  s2 = 0x20;
  __Func_8010704(0x1a, 0x1e, 1, 1, s1, s2);
  if (actor1 != 0)
  {
    actor2 = (struct Actor *) __MapActor_GetActor(0x10);
    actor2->interactFlag = 0;
    actor1->flags = 1;
  }
  __SetFlag(0x200);
}
