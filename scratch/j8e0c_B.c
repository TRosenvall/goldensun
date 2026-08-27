
#include "gba/types.h"
#include "actor.h"

extern void __MapActor_SetAnim(int slot, int anim);
extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_898_2008e0c(void)
{
    Actor *a;
    u8 *p;
    int slot;

    slot = 0x13;
    a = __MapActor_GetActor(0x13);
    p = (u8 *)a + 0x5b;
    *p = 1;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0) {
        __MessageID(0x1241);
        __MapActor_SetAnim(0x13, 0);
        __CutsceneWait(2);
    } else if (__GetFlag(0x858) != 0) {
        __MessageID(0x13ab);
    } else {
        __MessageID(0x134e);
    }
    __ActorMessage(slot, 0);
    __CutsceneEnd();
    *p = 0;
}
