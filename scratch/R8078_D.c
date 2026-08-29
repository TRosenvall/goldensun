#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern unsigned char ActorCmd_ARRAY_966__02009638[];
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_966_2008078(int slot)
{
    Actor *a;
    fx32 v;
    int x, z;

    a = __MapActor_GetActor(slot);
    v = 0x80 << 9;
    a->rotX = v;
    a = __MapActor_GetActor(slot);
    a->rotY = v;
    x = 0xc0 << 8;
    z = 0;
    __MessageID(0x26af);
    __ActorMessage(slot, 0);
    __Func_8092adc(slot, x, z);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(slot, ActorCmd_ARRAY_966__02009638);
}
