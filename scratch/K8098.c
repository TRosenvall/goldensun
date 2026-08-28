#include "gba/types.h"
#include "actor.h"

extern struct Actor *__CreateActor(int kind, fx32 x, fx32 y, fx32 z);
extern void OvlFunc_968_2008030(struct Actor *a, int n);

struct Actor *OvlFunc_968_2008098(fx32 x, fx32 y, fx32 z, int kind)
{
    struct Actor *n;
    unsigned char *s;
    int m;

    n = __CreateActor(kind, x, y, z);
    if (n == 0)
        return 0;
    s = (unsigned char *)n->sprite;
    m = -0xd;
    s[9] = (s[9] & m) | 4;
    OvlFunc_968_2008030(n, 0xf);
    n->flags |= 2;
    return n;
}
