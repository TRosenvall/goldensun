/* Cluster Func_809a65c..Func_809a65c extracted from goldensun/asm/rom_8a000/rom_9a44c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_8a000/rom_9a44c_a_a_a.o and asm/rom_8a000/rom_9a44c_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * The MAIN-ROM original of src/overlays/rom_7a5214/ovl_17ec_c_b.c, a per-frame
 * integrator with damping: add the stored velocities to the position, bleed
 * 1/0x12 off the X velocity and 1/16 off the Z, add the angular velocities and
 * advance the sprite's angle. That overlay function has five byte-identical
 * copies across five overlays; this is the sixth copy, in the main ROM.
 *
 * IT NEEDS NO LINKER ALIAS, and that is the interesting part. The overlay
 * copies call `_divsi3_RAM`, a RAM-resident copy of gcc's division helper, and
 * each overlay's linker script carries `__divsi3 = _divsi3_RAM;` to point
 * gcc's emitted call at it. Main-ROM code calls `__divsi3` directly, which is
 * what gcc emits unaided.
 *
 * That is independent support for the alias being right rather than a
 * convenient fiction: the same source, compiled into the main ROM, resolves to
 * the ordinary helper, and compiled into an overlay resolves to the RAM one.
 * The `FOR REVIEW` note on the overlay file can be read with that in mind.
 */
#include "actor.h"

struct DrawActor {
    u8 pad_00[0x1e];
    u16 angle;
};

void Func_809a65c(Actor *a)
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
