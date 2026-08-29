/* Cluster OvlFunc_969_200a1ac..OvlFunc_969_200a1ac extracted from goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_a_a_c_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * The mirror half of the 32-frame sine effect pair, one of seven identical
 * copies. See src/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_b.c for the three
 * differences from its partner, and
 * src/overlays/rom_77a7c8/ovl_30_c_c_c_c_b.c for why the counter is read back
 * through a signed 16-bit narrowing.
 */
#include "actor.h"

extern void __DeleteActor(Actor *a);
extern int __sin(int angle);

void OvlFunc_969_200a1ac(Actor *a)
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
