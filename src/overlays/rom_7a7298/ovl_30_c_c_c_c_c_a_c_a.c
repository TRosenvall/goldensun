#include "gba/types.h"
#include "actor.h"

extern void __Func_80929d8(struct Actor *a, int n);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);

void OvlFunc_921_2009704(struct Actor *a)
{
    unsigned char *s;
    int m;
    int v;

    a->interactFlag = 0;
    a->goalFacing = 0;
    a->flags &= 0xfe;
    s = (unsigned char *)a->sprite;
    m = -0xd;
    s[9] = (s[9] & m) | 4;
    __Func_80929d8(a, 9);
    __Actor_SetSpriteFlags(a, 0);
    v = 0x80 << 8;
    a->rotX = v;
    a->rotY = v;
}
