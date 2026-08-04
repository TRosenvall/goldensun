/* Cluster LoadStatusIcon..LoadStatusIcon extracted from goldensun/asm/rom_15000/rom_19ebc_a_c_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_19ebc_a_c_c_c_a_c_a.o and asm/rom_15000/rom_19ebc_a_c_c_c_a_c_c.o in
 * goldensun/stage1.ld.
 *
 * THIS WAS PARKED, and the park note was wrong about what it would take.
 *
 * It sat at 26 of 27 instructions on the arg-fill-order class: the ROM fills
 * the three argument registers for UploadSpriteGFX in the order r1, r2, r0,
 * putting the destination in r0 LAST, and gcc filled r0 first.
 *
 *     rom    mov r1, #0x80 / mov r2, r5    / mov r0, r8
 *     ours   mov r0, r8    / mov r1, #0x80 / mov r2, r5
 *
 * The note recorded three failed attempts -- the source pointer in its own
 * local, inline in the call, and the destination read into a fresh local
 * first -- and concluded "the order does not move".
 *
 * IT MOVES WHEN THE CALLEE ITSELF IS LEFT IMPLICITLY DECLARED. Dropping the
 * `extern void UploadSpriteGFX(...)` line matches on the first screen.
 *
 * That is a WIDER lever than the one docs/elevation.md describes. The
 * documented form is about the PRECEDING call: an implicitly declared callee
 * returns int, so gcc keeps r0 live across it and fills the next call's r0
 * last. That form was tried here too (DecompressStatusIcon left implicit) and
 * it does NOT work. What works is making the call in question implicit, which
 * changes the order gcc fills that call's OWN arguments. Two different
 * mechanisms wearing the same clothes; only the second one reaches this.
 *
 * Worth keeping from the original note: the second parameter is genuinely
 * unused. r1 is overwritten with 0x608 before anything reads it, so the
 * signature really is (index, unused, dest).
 *
 * Its sibling LoadInventoryIcon in rom_19ebc_a_c_c_c_a_b.c is the same
 * function with a different decompressor and matched the same way.
 */
#include "gba/types.h"

extern u8 *galloc_iwram(s32 tag, s32 size);
extern void gfree(s32 tag);

/* Allocates the tag-0x11 scratch, decompresses the icon set into it, uploads
 * 0x80 bytes from +0x400, and releases the arena. Always returns 1.
 */
s32 LoadStatusIcon(s32 index, s32 unused, s32 dest)
{
    u8 *buffer = galloc_iwram(0x11, 0x608);

    DecompressStatusIcon(index);
    UploadSpriteGFX(dest, 0x80, buffer + 0x400);
    gfree(0x11);
    return 1;
}
