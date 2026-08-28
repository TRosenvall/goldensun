/* Cluster OvlFunc_949_2008ca8..OvlFunc_949_2008ca8 extracted from goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_c_c.s.
 *
 * Total .text for this TU computed at build time from expected/.../.o.
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d4af4/ovl_30_c_c_c_c_a.o and asm/overlays/rom_7d4af4/ovl_30_c_c_c_c_c.o in
 * goldensun/overlays/rom_7d4af4/overlay.ld.
 */
#include "actor.h"

void OvlFunc_949_2008ca8(void)
{
  struct Actor *actor;
  unsigned long long t;
  unsigned long v;
  int s1;
  int s2;

  actor = (struct Actor *) __MapActor_GetActor(9);
  if (actor != 0)
  {
    __Actor_SetSpriteFlags(actor, 0);
    actor->flags = 2;
    actor->interactFlag = 0;
  }
  t = 9;
  do { t = (unsigned long) t; } while (0);
  v = t;
  __MapActor_SetAnim(v, 5);
  s1 = 0x22;
  s2 = 0x10;
  __Func_8010704(0x24, 0x10, 1, 1, s1, s2);
  __SetFlag(0x201);
}
