#include "actor.h"

extern int __atan2(int dz, int dx);
extern int OvlFunc_928_20083cc(int *a, int *b);
extern void __Actor_SetAnim(Actor *a, int n);

int OvlFunc_928_2008408(Actor *a, Actor *b, int lim, int force)
{
    int ret;
    unsigned short ang;
    int m0;
    int m1;
    int m2;
    int cur;

    ret = 0;
    if (a->stop == 1) {
        if (*(short *)&a->goalFacing == 0) {
            __Actor_SetAnim(a, 1);
            return 1;
        }
    }
    if (OvlFunc_928_20083cc((int *)&b->pos, (int *)&a->pos) < lim || force != 0) {
        ang = __atan2(b->pos.z - a->pos.z, b->pos.x - a->pos.x);
        m2 = (ang - 0x1000) & 0xf000;
        m1 = (ang + 0x1000) & 0xf000;
        m0 = ang & 0xf000;
        cur = a->facing & 0xf000;
        if (m0 == cur || m1 == cur || m2 == cur || force != 0) {
            a->stop = 1;
            __Actor_SetAnim(a, 1);
            ret = 1;
            a->goalFacing = 1;
        } else {
            a->stop = 0;
            __Actor_SetAnim(a, 2);
            a->goalFacing = 0;
        }
    } else {
        a->stop = 0;
        __Actor_SetAnim(a, 2);
        a->goalFacing = 0;
    }
    return ret;
}
