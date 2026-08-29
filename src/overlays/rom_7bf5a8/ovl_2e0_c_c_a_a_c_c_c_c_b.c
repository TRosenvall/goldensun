/* Cluster OvlFunc_935_2008640..OvlFunc_935_2008640 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c.s.
 *
 * Total .text for this TU = 80 bytes (= 0x50).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c_a.o and asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c_c.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld.
 */
#include "actor.h"
extern void *__MapActor_GetActor(int id);
extern void __Actor_SetSpriteFlags(void *actor, int flags);
extern void __Func_8010704(int a0, int a1, int a2, int a3, int a4, int a5);
extern void __SetFlag(int flag);

void OvlFunc_935_2008640(void)
{
  struct Actor *actor;
  struct Actor *actor2;
  int s1;
  int s2;

  actor = (struct Actor *) __MapActor_GetActor(0x14);
  s1 = 0x1a;
  s2 = 0x22;
  __Func_8010704(0x1a, 0x1e, 1, 1, s1, s2);
  if (actor != 0)
  {
    __Actor_SetSpriteFlags(actor, 0);
    actor2 = (struct Actor *) __MapActor_GetActor(0x14);
    actor2->interactFlag = 0;
    actor->flags = 1;
  }
  __SetFlag(0x204);
}
