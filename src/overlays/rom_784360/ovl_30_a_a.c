/* Cluster OvlFunc_884_2008030..OvlFunc_884_2008030 extracted from goldensun/asm/overlays/rom_784360/ovl_30_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Turns an actor one step toward its target: takes the angle to the target
 * with atan2, clamps the change to +/-0x1000 of a full circle, and applies it.
 * Clears walkFlags bit 0 -- "turn to face the target" -- on the way, since the
 * turn is now being done explicitly.
 *
 * Always returns 1; the early exits share the same tail.
 *
 * THE WIDTH OF `ang` IS LOAD-BEARING. The ROM zero-extends the atan2 result to
 * sixteen bits BEFORE subtracting the current facing:
 *
 *     lsl r0, #16 / lsr r0, #16 / sub r0, r3 / lsl r0, #16 / asr r0, #16
 *
 * Written with `ang` as an int and the cast in the expression --
 * `(short)((unsigned short)ang - cur)` -- gcc drops the first pair, because
 * masking before a subtraction whose result is truncated to sixteen bits
 * anyway is redundant. It is right, and it is two instructions shorter than
 * the ROM. Declaring `ang` as an unsigned short forces the narrowing to happen
 * at the assignment, where the ROM has it.
 *
 * Declaring __atan2 as RETURNING unsigned short matches equally well, so this
 * function does not settle which the original did. The local is used here
 * because it claims less: __atan2 is shared, and its signature should be
 * decided by a function that actually pins it.
 */
#include "actor.h"

extern int __atan2(int dz, int dx);

int OvlFunc_884_2008030(Actor *a)
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
