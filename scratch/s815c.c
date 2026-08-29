#include "gba/types.h"
#include "actor.h"

extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);
extern void __Func_80b0278(int shop, int slot);

void OvlFunc_967_200815c(int slot)
{
    Actor *a;
    u32 f;
    int m;

    a = __MapActor_GetActor(0);
    f = a->facing;
    f += 0x80 << 6;
    m = ~0x3fff;
    if ((u16)(f & m) == 0xc000) {
        __Func_80b0278(0x21, slot);
    } else if (__GetFlag(0x9a7)) {
        __MessageID(0x28f2);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x26e7);
        __ActorMessage(slot, 0);
    }
}
