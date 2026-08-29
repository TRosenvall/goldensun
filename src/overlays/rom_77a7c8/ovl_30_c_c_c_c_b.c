/* Cluster OvlFunc_881_200bfb4..OvlFunc_881_200bfb4 extracted from goldensun/asm/overlays/rom_77a7c8/ovl_30_c_c_c_c.s.
 *
 * Split out of that .s; the _a part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_77a7c8/overlay.ld.
 *
 * A 32-frame effect actor riding on a parent: each frame it advances a counter
 * in goalFacing, deletes itself once past 0x1f, and otherwise takes sin() of
 * the counter scaled to a full circle and uses it for both the scale/rotation
 * pair and the height offset. x tracks the parent exactly, y climbs a fixed
 * 0x10000 per frame, and z sits above the parent by (0x10000 - sin) * 5 plus a
 * constant 0x80000.
 *
 * HEAD OF A 7-MEMBER FAMILY.
 *
 * `n` is read back through a SIGNED 16-bit narrowing after the increment --
 * the ROM does `lsl #16 / asr #16` on the stored halfword rather than using
 * the value it just computed. Reading `a->goalFacing` again (a u16 field)
 * and casting to short is what produces that; keeping the incremented value
 * in an int does not.
 *
 * The 0x18/0x1C pair is named rot* here after include/actor.h, which records
 * that the draw path reads the same pair as a scale. This function does not
 * settle it: a 32-frame sine applied to both members at once fits either
 * reading.
 */
#include "actor.h"

extern void __DeleteActor(Actor *a);
extern int __sin(int angle);

void OvlFunc_881_200bfb4(Actor *a)
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
    a->rotY = s;
    a->pos.x = t->pos.x;
    a->pos.y += 0x80 << 9;
    a->pos.z = t->pos.z + ((0x80 << 9) - s) * 5 + (0x80 << 12);
}
