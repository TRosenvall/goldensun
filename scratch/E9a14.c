#include "gba/types.h"
#include "actor.h"

extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_2009a14(struct Actor *a)
{
    int x, z;

    a->flags |= 2;
    a->interactFlag = 0;
    x = a->pos.x >> 20;
    z = a->pos.z >> 20;
    __Func_8010704(9, 0x18, 1, 1, x, z);
}
