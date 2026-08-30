#include "actor.h"

extern int atan2(int dz, int dx);
extern void WaitFrames(int n);

void Func_8092878(Actor *a, Actor *b)
{
    int ang;
    int ang2;
    int i;
    int n;
    int cur;
    int d;

    if (a != 0 && b != 0) {
        ang = (unsigned short)atan2(b->pos.z - a->pos.z, b->pos.x - a->pos.x);
        ang2 = (0x80 << 8) + ang;
        i = 0;
        do {
            n = 2;
            cur = a->facing;
            d = (short)(ang - cur);
            if (d != 0) {
                if (d > 0x1000)
                    d = 0x1000;
                if (d < -0x1000)
                    d = -0x1000;
                a->facing = cur + d;
            } else {
                n = 1;
            }
            cur = b->facing;
            d = (short)(ang2 - cur);
            if (d != 0) {
                if (d > 0x1000)
                    d = 0x1000;
                if (d < -0x1000)
                    d = -0x1000;
                b->facing = cur + d;
            } else {
                n--;
            }
            if (n == 0)
                break;
            WaitFrames(1);
            i++;
        } while (i <= 0x3b);
    }
}
