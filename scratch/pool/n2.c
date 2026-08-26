#include "gba/types.h"
#include "actor.h"
extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);
void OvlFunc_962_200816c(int slot)
{
    Actor *a; u16 f; short m;
    a = __MapActor_GetActor(0);
    f = a->facing; f += 0x80 << 6; m = -0x4000;
    if ((u16)(f & m) == 0xc000) { __UI_Sanctum(slot); }
    else if (__GetFlag(0x96f)) { __MessageID(0x262c); __ActorMessage(slot, 0); }
    else { __MessageID(0x25d5); __ActorMessage(slot, 0); }
}
