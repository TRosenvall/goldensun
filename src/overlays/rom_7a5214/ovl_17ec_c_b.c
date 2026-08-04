/* Cluster OvlFunc_918_200985c..OvlFunc_918_200985c extracted from goldensun/asm/overlays/rom_7a5214/ovl_17ec_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a5214/ovl_17ec_c_a.o and asm/overlays/rom_7a5214/ovl_17ec_c_c.o in
 * goldensun/overlays/rom_7a5214/overlay.ld.
 *
 * A per-frame integrator with damping: add the stored velocities to the
 * position, bleed 1/0x12 off the X velocity and 1/16 off the Z, then add the
 * angular velocities and advance the sprite's angle.
 *
 * One of FIVE byte-identical copies, one per overlay -- OvlFunc_946_2008da4,
 * OvlFunc_964_2009068, OvlFunc_965_2008cf0, OvlFunc_968_200896c are the others.
 * Found with tools/find_twins.py.
 *
 * THE ONLY DIFFICULTY IS WHICH DIVISION HELPER GETS CALLED, and the fix is at
 * the LINK, not in the C.
 *
 * The C below is a plain transcription and screens with exactly one difference:
 *
 *     rom    bl _divsi3_RAM
 *     ours   bl __divsi3
 *
 * Those are DIFFERENT FUNCTIONS AT DIFFERENT ADDRESSES in this ROM -- 348 call
 * sites use __divsi3 and 104 use _divsi3_RAM -- so this is not a naming
 * cosmetic. Overlay code calls a RAM-resident copy of the divide routine, and
 * `_divsi3_RAM` is the import stub that `.export_func divsi3_RAM` generates in
 * every overlay's imports.s.
 *
 * gcc-2.96 emits `__divsi3` for `/` and there is no flag to rename it. So the
 * overlay's linker script now carries
 *
 *     __divsi3 = _divsi3_RAM;
 *
 * which is an absolute alias -- it emits no bytes and points gcc's helper at
 * the stub the ROM already calls. The linked bytes are identical.
 *
 * WHY THIS RATHER THAN CALLING _divsi3_RAM DIRECTLY FROM C. Writing
 * `vx -= _divsi3_RAM(vx, 0x12)` also emits the right instruction, but it makes
 * gcc treat the division as an ordinary call rather than a libgcc helper, and
 * the scheduling changes: a load moves above a store two statements earlier and
 * the function no longer matches. The alias keeps `/` in the source, which is
 * almost certainly what Camelot wrote, and keeps gcc's own idea of what a
 * division costs.
 *
 * FOR REVIEW: this is an assumption about the original build -- that overlay
 * translation units resolved gcc's integer-division helpers to the RAM-resident
 * copies. It is consistent with every overlay's imports.s already exporting
 * divsi3_RAM and udivsi3_RAM, and with no overlay calling __divsi3 anywhere in
 * the ROM. If that is wrong the alias is the thing to revisit.
 */
#include "actor.h"

struct DrawActor {
    u8 pad_00[0x1e];
    u16 angle;
};

void OvlFunc_918_200985c(Actor *a)
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
