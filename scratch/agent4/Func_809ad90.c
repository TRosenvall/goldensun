#include "actor.h"

extern unsigned char gState[];
extern Actor *GetFieldActor(s32 slot);
extern void Func_809ad70(Actor *actor);
extern void _Actor_SetAnimSpeed(Actor *actor, s32 speed);

void Func_809ad90(s32 slot)
{
    Actor *a;
    unsigned char *g;
    unsigned char *s;

    a = GetFieldActor(slot);
    if (a == 0)
        return;
    g = gState;
    *(actorfun_t *)(g + (0x94 << 2)) = a->update;
    g += 0x249;
    *g = 0;
    if (a->drawKind == 1) {
        s = *(unsigned char **)((unsigned char *)a->sprite + 0x28);
        if (s != 0)
            *g = s[5];
    }
    a->update = Func_809ad70;
    a->stop = 1;
    _Actor_SetAnimSpeed(a, 0);
}
