#include "gba/types.h"
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);

void OvlFunc_947_200a5f8(int slot)
{
    struct Actor *a;
    unsigned char *s;
    int m;

    a = __MapActor_GetActor(slot);
    a->interactFlags &= 0xfe;
    a->flags |= 2;
    a->interactFlag = 0;
    __Actor_SetSpriteFlags(a, 0);
    s = (unsigned char *)a->sprite;
    m = -0xd;
    s[9] = (s[9] & m) | 8;
}
