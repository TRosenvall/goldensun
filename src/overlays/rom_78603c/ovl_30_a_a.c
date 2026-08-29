/* Cluster OvlFunc_885_2008030..OvlFunc_885_2008030 extracted from goldensun/asm/overlays/rom_78603c/ovl_30_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Turn one step toward the target, one of ten identical copies -- one per
 * overlay, byte-for-byte the same body. See
 * src/overlays/rom_784360/ovl_30_a_a.c for why the angle has to be an
 * unsigned short local, which is the only subtle thing here.
 */
#include "actor.h"

extern int __atan2(int dz, int dx);

int OvlFunc_885_2008030(Actor *a)
{
    Actor *t;
    unsigned short ang;
    int cur;
    int d;

    t = (Actor *)a->unk_68;
    if (t != 0) {
        a->walkFlags &= ~1;
        ang = __atan2(t->pos.z - a->pos.z, t->pos.x - a->pos.x);
        cur = a->facing;
        d = (short)(ang - cur);
        if (d != 0) {
            if (d > 0x1000)
                d = 0x1000;
            if (d < -0x1000)
                d = -0x1000;
            a->facing = cur + d;
        }
    }
    return 1;
}
