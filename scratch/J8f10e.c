#include "gba/types.h"
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);

void OvlFunc_957_2008f10(int slot, int b, int c)
{
    struct Actor *a;
    int v[3];
    int *p;
    int k1, k2, k3;

    k1 = 0xfc << 17;
    k2 = 0xc0 << 13;
    a = __MapActor_GetActor(slot);
    p = v;
    p[0] = k1;
    p[2] = k2;
    __vec3_translate(b, c, p);
    a->pos.x = p[0];
    k3 = 0x90 << 16;
    a->pos.y = p[2];
    a->pos.z = k3;
}
