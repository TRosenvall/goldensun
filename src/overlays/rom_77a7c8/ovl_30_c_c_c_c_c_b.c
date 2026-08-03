/* Cluster OvlFunc_881_200c004..OvlFunc_881_200c004 extracted from goldensun/asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_77a7c8/overlay.ld.
 *
 * The MIRROR of OvlFunc_881_200bfb4, which sits immediately before it in the
 * same file. Same 32-frame sine, same parent tracking, three differences:
 *
 *     rotY  = -sin  rather than +sin
 *     z     = parent - (0x10000 - sin) * 5   rather than +
 *     offset  0x100000  rather than 0x80000
 *
 * The two are laid down as a pair in all seven overlays that have them, which
 * is what a symmetric effect -- one on each side of the parent -- looks like.
 *
 * HEAD OF THE COMPANION 7-MEMBER FAMILY. See
 * src/overlays/rom_77a7c8/ovl_30_c_c_c_c_b.c for why the counter is read back
 * through a signed 16-bit narrowing, and for what the 0x18/0x1C pair is and is
 * not known to be.
 */
#include "actor.h"

extern void __DeleteActor(Actor *a);
extern int __sin(int angle);

void OvlFunc_881_200c004(Actor *a)
{
    Actor *t;
    int n;
    int s;

    t = (Actor *)a->unk_68;
    a->goalFacing++;
    n = (short)a->goalFacing;
    if (n > 0x1f) {
        __DeleteActor(a);
        return;
    }
    s = __sin(n << 10);
    a->rotX = s;
    a->rotY = -s;
    a->pos.x = t->pos.x;
    a->pos.y += 0x80 << 9;
    a->pos.z = t->pos.z - ((0x80 << 9) - s) * 5 + (0x80 << 13);
}
