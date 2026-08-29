/* NewActor @ 0x0800c0cc
 *
 * Source asm: goldensun/asm/rom_9000/rom_c004_c_a_a_a_a_a_a.s
 *
 * NOT SPLIT. The .s still holds all three of its functions.
 *
 * Scans up to 64 actor slots on a 0x70 stride for one whose first word is
 * zero, returning its address or NULL.
 *
 * TWENTY INSTRUCTIONS AGAINST TWENTY, and the whole diff is that two registers
 * are swapped:
 *
 *     rom    ldr r2, [r3] / ldr r3, [r2]   pointer -> r2, loaded value -> r3
 *     ours   ldr r3, [r3] / ldr r2, [r3]   pointer -> r3, loaded value -> r2
 *
 * and the swap then runs through the whole loop (`add r2, #0x70` against
 * `add r3, #0x70`).
 *
 * Blocker: ADDRESS-REGISTER REUSE. gcc loads the pointer into the SAME register
 * that held the symbol address and reuses it; the ROM allocates a fresh
 * register and leaves the address register dead. This is the fourth function
 * with that residue -- see also src/non_matching/rom_c0/rom_5868.c,
 * src/non_matching/rom_8a000/rom_91c44.c and
 * src/non_matching/rom_b5000/rom_c00d8.c, where the same thing shows up as an
 * extra `mov` because the ROM stages through a scratch before moving to a
 * callee-saved register.
 *
 * Note it does NOT appear when the pointer must be callee-saved because it is
 * live across a call -- src/non_matching/rom_9000/rom_12350.c has the same
 * loop shape and both sides pick r5. So it is specific to a pointer that stays
 * in the caller-clobbered set.
 *
 * The goto-loop lever gets the loop skeleton exactly right here; without it the
 * shape is further out.
 *
 * TRIED, all byte-identical to each other:
 *   1. the two returns collapsed into one exit label (this form) vs an early
 *      `return r` in the loop -- the early return costs a line, 19 of 20
 *   2. three declaration orders for the pointer, value and result locals
 *   3. `extern u8 *iwram_3001e64` so the read needs no cast, rather than
 *      `extern u32` plus a cast
 *
 * The stride is 0x70, which is NOT sizeof(Actor) -- include/actor.h describes a
 * 0x80-byte struct whose last two fields are marked GS1-only. So this array is
 * either the GS2 layout or a different structure entirely, and the pointer is
 * kept as a byte pointer rather than asserting a type.
 */
#include "gba/types.h"

extern u32 iwram_3001e64;

u8 *NewActor(void)
{
    u8 *p;
    u8 *r;
    u32 s;
    s32 i;

    p = (u8 *)iwram_3001e64;
    s = *(u32 *)p;
    r = 0;
    i = 0;
    goto check;
loop:
    i++;
    p += 0x70;
    if (i > 0x3f)
        goto out;
    s = *(u32 *)p;
check:
    if (s != 0)
        goto loop;
    r = p;
out:
    return r;
}
