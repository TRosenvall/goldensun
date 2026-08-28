#include "gba/types.h"
#include "actor.h"

extern unsigned int __Random(void);

int OvlFunc_945_2008058(struct Actor *a)
{
    short *f;
    unsigned int u;
    int v;

    f = (short *)((char *)a + 0x66);
    if (*f != 0) {
        u = __Random();
        v = a->pos.y;
        v -= (u << 15) >> 16;
        v += 0xffff8000;
        a->pos.y = v;
        if (v < 0)
            *f = 0;
    } else {
        u = __Random();
        v = a->pos.y;
        v += (u << 15) >> 16;
        v += 0x80 << 8;
        a->pos.y = v;
        if (v > (0x80 << 12))
            *f = 1;
    }
    return 1;
}
