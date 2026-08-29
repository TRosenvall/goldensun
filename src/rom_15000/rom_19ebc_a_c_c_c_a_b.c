/* Cluster LoadInventoryIcon..LoadInventoryIcon extracted from goldensun/asm/rom_15000/rom_19ebc_a_c_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_19ebc_a_c_c_c_a_a.o and asm/rom_15000/rom_19ebc_a_c_c_c_a_c.o in
 * goldensun/stage1.ld.
 *
 * The same function as LoadStatusIcon next door: allocate the tag-0x11
 * scratch, draw into it, upload 0x80 bytes from +0x400, release the arena,
 * return 1. This one draws an inventory icon and so takes a second argument
 * that LoadStatusIcon leaves unused.
 *
 * Matched by the same lever, and see rom_19ebc_a_c_c_c_a_c_b.c for why it is
 * not the lever docs/elevation.md describes: UploadSpriteGFX is left
 * IMPLICITLY DECLARED on purpose, which reverses the order gcc fills that
 * call's own argument registers and puts the destination in r0 last, as the
 * ROM does. Adding the extern back costs the match.
 *
 * DrawInventoryIcon is also implicit, but only because it has no declaration
 * anywhere in the tree yet -- it is defined earlier in this same .s and is
 * not load-bearing for the match here.
 */
#include "gba/types.h"

extern u8 *galloc_iwram(s32 tag, s32 size);
extern void gfree(s32 tag);

s32 LoadInventoryIcon(s32 index, s32 arg, s32 dest)
{
    u8 *buffer = galloc_iwram(0x11, 0x608);

    DrawInventoryIcon(index, arg);
    UploadSpriteGFX(dest, 0x80, buffer + 0x400);
    gfree(0x11);
    return 1;
}
