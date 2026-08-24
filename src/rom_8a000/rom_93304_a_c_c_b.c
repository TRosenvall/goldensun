/* Cluster Func_8093a14..Func_8093a14 extracted from goldensun/asm/rom_8a000/rom_93304_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_8a000/rom_93304_a_c_c_a.o and asm/rom_8a000/rom_93304_a_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * The MAIN-ROM original of src/overlays/rom_784360/ovl_30_a_a.c: turn an actor
 * one step toward its target, clamped to +/-0x1000. The overlay copy calls the
 * imported `__atan2`; this one calls `atan2` directly, and is otherwise the
 * same function.
 *
 * That is worth noting for its own sake -- the overlay is carrying a duplicate
 * of main-ROM code rather than calling into it, which is a thing this ROM does
 * and which tools/match_shapes.py is good at surfacing because it collapses
 * callee names.
 *
 * THE WIDTH OF `ang` IS LOAD-BEARING and the exemplar has the account: the ROM
 * zero-extends the atan2 result to sixteen bits BEFORE subtracting the current
 * facing, and only an `unsigned short` local puts the narrowing there. With an
 * int and a cast in the expression gcc drops the pair -- correctly, and two
 * instructions shorter than the ROM.
 */
#include "actor.h"

extern int atan2(int dz, int dx);

int Func_8093a14(Actor *a)
{
    Actor *t;
    unsigned short ang;
    int cur;
    int d;

    t = (Actor *)a->unk_68;
    if (t != 0) {
        a->walkFlags &= ~1;
        ang = atan2(t->pos.z - a->pos.z, t->pos.x - a->pos.x);
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
