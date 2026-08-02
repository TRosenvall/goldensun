/* LoadStatusIcon @ 0x0801a2ec -- asm/rom_15000/rom_19ebc_a_c_c.s
 *
 * Blocker class 5, SCHEDULING, in argument setup. 26 of 27 instructions
 * match; the three argument registers are filled in a different order:
 *
 *     rom    mov r1, #0x80 / mov r2, r5 / mov r0, r8
 *     ours   mov r0, r8    / mov r1, #0x80 / mov r2, r5
 *
 * The ROM fills r0 LAST, from the callee-saved register holding the
 * destination; gcc fills it first. Tried: the source pointer in its own local
 * assigned before the call, inline in the call, and with the destination read
 * into a fresh local immediately beforehand. The order does not move.
 *
 * Worth noting the second parameter is genuinely unused -- r1 is overwritten
 * with 0x608 before anything reads it -- so the signature really is
 * (index, unused, dest).
 */
#include "gba/types.h"

extern u8 *galloc_iwram(s32 tag, s32 size);
extern void DecompressStatusIcon(s32 index);
extern void UploadSpriteGFX(s32 dest, s32 size, void *src);
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
