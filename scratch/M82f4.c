#include "gba/types.h"
#include "actor.h"

extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Func_80929d8(struct Actor *a, int n);

int OvlFunc_945_20082f4(struct Actor *a)
{
    unsigned char *s;
    int m;

    a->interactFlags = 8;
    s = (unsigned char *)a->sprite;
    __Actor_SetSpriteFlags(a, 0);
    m = -0xd;
    s[9] = (s[9] & m) | 4;
    s[0x15] = (s[0x15] & m) | 4;
    a->flags = (a->flags & 0xfe) | 2;
    __Func_80929d8(a, 0xf);
    return 1;
}
