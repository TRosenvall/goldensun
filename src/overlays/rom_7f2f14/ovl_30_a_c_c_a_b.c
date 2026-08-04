/* Cluster OvlFunc_968_200896c..OvlFunc_968_200896c extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f2f14/ovl_30_a_c_c_a_a.o and asm/overlays/rom_7f2f14/ovl_30_a_c_c_a_c.o in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * One of five byte-identical copies of the per-frame integrator, one per
 * overlay. See src/overlays/rom_7a5214/ovl_17ec_c_b.c for the full account --
 * in particular why `__divsi3 = _divsi3_RAM;` is in this overlay's linker
 * script, and why the velZ update is written as a temp while the velX one is
 * in place.
 *
 * Found with tools/find_twins.py; the C is that file's with the name changed.
 */
#include "actor.h"

struct DrawActor {
    u8 pad_00[0x1e];
    u16 angle;
};

void OvlFunc_968_200896c(Actor *a)
{
    fx32 vx;
    fx32 vz;

    vx = a->velX;
    a->pos.x += vx;
    a->pos.y += a->velY;
    vz = a->velZ;
    a->pos.z += vz;
    vx -= vx / 0x12;
    a->velX = vx;
    a->velZ = vz - vz / 16;
    a->rotX += a->speed;
    a->rotY += a->accel;
    ((struct DrawActor *)a->sprite)->angle += a->goalFacing;
}
