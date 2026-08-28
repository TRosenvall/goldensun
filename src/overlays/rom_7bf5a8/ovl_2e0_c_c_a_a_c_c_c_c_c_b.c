/* Cluster OvlFunc_935_2008690..OvlFunc_935_2008690 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c_c.s.
 *
 * Total .text for this TU computed at build time from expected/.../.o.
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c_c_a.o and asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld.
 */
/* Cluster OvlFunc_935_2008690..OvlFunc_935_2008690 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c.s.
 *
 * Total .text for this TU = 84 bytes (= 0x54).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_a.o and asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_c_c_c_c.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld.
 */
#include "actor.h"
extern void __Actor_SetSpriteFlags(struct Actor *actor, int flags);

void OvlFunc_935_2008690(void)
{
  struct Actor *actor;
  int s1;
  int s2;

  actor = (struct Actor *) __MapActor_GetActor(0x15);
  s1 = 0x1c;
  s2 = 0x21;
  __Func_8010704(0x1a, 0x1e, 1, 1, s1, s2);
  if (actor != 0)
  {
    __Actor_SetSpriteFlags(actor, 0);
    ((struct Actor *) __MapActor_GetActor(0x15))->interactFlag = 0;
    actor->flags = 1;
  }
  __SetFlag(0x205);
}
