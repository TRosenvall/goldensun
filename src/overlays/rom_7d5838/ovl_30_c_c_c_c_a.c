/* OvlFunc_950_200891c  --  0x0200891c
 *
 * The function half of goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_c_c.s; the
 * .data half (nine .incbin blobs and the ten labels the rest of the overlay
 * branches into) stays behind in ovl_30_c_c_c_c_b.s, and the linker script
 * lists the two objects where the one used to be, in both the .text and the
 * .data run.
 *
 * A sanctum counter. If the player is standing in the right quadrant relative
 * to the attendant, open the sanctum UI; otherwise say one of three lines
 * depending on how far the story has got.
 *
 * THE FACING TEST IS A FOURTH SPELLING, and it belongs in the batch-29 table:
 *
 *      ldrh r3, [r0, #6] / add r3, #0x2000
 *      ldr r2, =0xffffc000 / and r3, r2
 *      lsl r3, #16 / cmp r3, #0xc0000000 / bne
 *
 * The three spellings already catalogued are RANGE tests -- `f - k <= n`. This
 * one is a QUADRANT test: mask the rotated angle down to its top two bits and
 * compare for equality. Two readings make it:
 *
 *   the mask is `~0x3fff`, NOT `0xc000`. gcc pools 0xffffc000 in one `ldr`
 *   where 0xc000 costs `mov` + `lsl`; writing the mask as the complement of a
 *   small constant is what puts the 32-bit form in the pool. (`& 0xc000`
 *   compiles to the two-instruction build and diverges at instruction 8.)
 *
 *   the result is an `unsigned short`. `lsl r3, #16` against a pre-shifted
 *   0xc0000000 is the narrowing-cast tell from batch 29, here on an equality
 *   rather than on a range. Keeping the value an `int` drops both shifts.
 *
 * Either the named local below or the direct expression
 * `(unsigned short)((a->f6 + 0x2000) & ~0x3fff) == 0xc000` matches; the local
 * is written out because it is what makes the 16-bit compare legible.
 *
 * The three message paths are written as three plain blocks. gcc cross-jumps
 * the first two into a shared `__MessageID / __ActorMessage` tail by itself --
 * that is the ROM's `b .L968` -- and leaves the third with its own copy, so
 * there is nothing to spell for it.
 */
struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __UI_Sanctum(int slot);

void OvlFunc_950_200891c(int slot)
{
    struct A *a;
    unsigned short d;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __UI_Sanctum(slot);
    } else if (__GetFlag(0x95 << 4)) {
        __MessageID(0x23bf);
        __ActorMessage(slot, 0);
    } else if (__GetFlag(0x962)) {
        __MessageID(0x2231);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x1feb);
        __ActorMessage(slot, 0);
    }
}
