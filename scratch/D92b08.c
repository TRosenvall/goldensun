#include "gba/types.h"
#include "actor.h"

extern struct Actor *GetFieldActor(int slot);

void Func_8092b08(int slot, int prio)
{
    struct Actor *a;
    unsigned char *s;
    int m, v;

    a = GetFieldActor(slot);
    if (a != 0) {
        if ((a->drawKind & 0xf) == 1) {
            s = (unsigned char *)a->sprite;
            m = -0xd;
            prio &= 3;
            v = prio << 2;
            s[9] = (s[9] & m) | v;
            s[0x15] = (s[0x15] & m) | v;
            a->flags &= 0xfe;
        }
    }
}
