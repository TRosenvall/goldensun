/* Cluster OvlFunc_common0_d4..OvlFunc_common0_d4 extracted from goldensun/asm/overlays/common/common0.s.
 *
 * Total .text for this TU = 54 bytes (= 0x36).
 * Slotted between the _a and _c pieces in all NINETEEN overlay.ld scripts
 * that name this object. The _c piece keeps the .data and .data1 sections.
 *
 * Advances an entity by its per-axis deltas and spins its attached sprite.
 * Three position words at 0x08/0x0c/0x10 take the deltas at 0x44/0x48/0x4c,
 * two angle words at 0x18/0x1c take the deltas at 0x30/0x34, and the sprite
 * pointed to from 0x50 has the halfword at its +0x1e advanced by the rate at
 * the entity's +0x64.
 *
 * SEVEN BYTE-IDENTICAL COPIES OF THIS FUNCTION EXIST -- one in the main ROM,
 * one in overlays/common, and five in per-area overlays. They are listed in
 * reports/batch-69.md. This C is shared verbatim; only the symbol changes.
 * This is the seventh, deferred from batch 69 because its object is named by
 * nineteen overlay linker scripts.
 *
 * THIS TU IS BUILT WITH -fno-strict-aliasing, and that is the whole reason it
 * matches. At -O2 gcc-2.96 turns strict aliasing on, which lets the post-reload
 * scheduler prove that loading `p->t` (a pointer) cannot conflict with storing
 * `p->b` (an int) and hoist the load two instructions earlier, into the
 * load-use stall:
 *
 *      ours   ldr r3, [r0, #0x1c] / ldr r1, [r0, #0x50] / add r3, r2 / str r3, [r0, #0x1c]
 *      rom    ldr r3, [r0, #0x1c] / add r3, r2 / str r3, [r0, #0x1c] / ldr r1, [r0, #0x50]
 *
 * With -fno-strict-aliasing the scheduler has to assume the store may alias
 * the load and leaves the order alone. Every other instruction in the function
 * already matched, including the src-before-dst load order in the five
 * accumulates, which IS the scheduler doing its job -- so the pass is wanted
 * here, only its alias information is not.
 *
 * The flag is per-TU and not global: applying it to GCC296_CFLAGS and
 * rebuilding every C translation unit leaves 2631 bytes differing across the
 * ROM. See the ALIAS_CFLAGS comment in the Makefile.
 */

struct T {
    unsigned char pad[0x1e];
    unsigned short ang;
};

struct S {
    /* 0x00 */ int pad0[2];
    /* 0x08 */ int x, y, z;
    /* 0x14 */ int pad14;
    /* 0x18 */ int a, b;
    /* 0x20 */ int pad20[4];
    /* 0x30 */ int da, db;
    /* 0x38 */ int pad38[3];
    /* 0x44 */ int dx, dy, dz;
    /* 0x50 */ struct T *t;
    /* 0x54 */ int pad54[4];
    /* 0x64 */ unsigned short spin;
};

void OvlFunc_common0_d4(struct S *p)
{
    p->x += p->dx;
    p->y += p->dy;
    p->z += p->dz;
    p->a += p->da;
    p->b += p->db;
    p->t->ang += p->spin;
}
