#include "gba/types.h"
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);

void OvlFunc_957_2008f10(int slot, int b, int c)
{
    struct Actor *a;
    int v[3];

    v[0] = 0xfc << 17;
    v[2] = 0xc0 << 13;
    a = __MapActor_GetActor(slot);
    __vec3_translate(b, c, v);
    a->pos.x = v[0];
    a->pos.y = v[2];
    a->pos.z = 0x90 << 16;
}
