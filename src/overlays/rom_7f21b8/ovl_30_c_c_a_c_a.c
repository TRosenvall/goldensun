#include "gba/types.h"
#include "actor.h"

extern int _MSG_26e3;
extern Actor *__MapActor_GetActor(int slot);
extern void __Func_80b0278(int a, int b);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern int __Func_8092c40(int actor, int b);
extern int __Func_8091c7c(int a, int b);
extern void __CutsceneWait(int n);

void OvlFunc_967_20080c8(int actor)
{
    Actor *a;
    u16 d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->facing + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x20, actor);
    } else if (__GetFlag(0x9a7)) {
        __MessageID(0x28f0);
        __ActorMessage(actor, 0);
    } else {
        base = (int)(&_MSG_26e3);
        __MessageID(base);
        __Func_8092c40(actor, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(actor, 0);
    }
}
