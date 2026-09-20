/* Cluster UploadBGPalette..UploadBGPalette extracted from goldensun/asm/rom_b5000/rom_c10e8_a_a.s.
 *
 * Total .text for this TU = 116 bytes (= 0x74).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_c10e8_a_a_b.o and asm/rom_b5000/rom_c10e8_a_a_c_c.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 272. No pins, no flags -- and it took 29 screens,
 * so all three levers are recorded.
 *
 * A per-channel palette fade: clamp the amount to 1.0 (0x10000), then for each
 * entry multiply the three 5-bit channels in place and take bits 16..20 of each
 * product back.
 *
 * 1. `r *= amount`, NOT `r = (c & 0x1f) * amount`. The one-statement form expands
 *    to `(mult amount masked)`, and Thumb's mulsi3 is DESTRUCTIVE, so regmove
 *    cannot tie the product to the dying mask and reload pays
 *    `mov r2, r4 / mul r2, r2, r3` at all three channels. Split into mask-then-`*=`
 *    and the mask pseudo IS the product pseudo: `mul r3, r0`, as the ROM has. Worth
 *    70 lines down to 60.
 *
 * 2. `c` IS ASSIGNED A SECOND TIME and the accumulator is that second assignment.
 *    With a separate result local, .17.lreg reports `Register 36 used 8 times ... in
 *    block 4; set 1 time` and local-alloc gives it r0; .18.greg then has hard reg 0
 *    in `amount`'s conflict set, so `amount` is pushed to r4, only r1/r2/r3 remain
 *    as loop scratch, the three products spill to r8/r9/r10 and the prologue grows
 *    `mov r6, r9 / mov r5, r8`. Reusing `c` gives `set 2 times; dies in 2 places`,
 *    which local-alloc CANNOT form into one quantity, so it falls through to
 *    global-alloc, lands in r4, and `amount` gets r0 -- the ROM's deal.
 *
 * 3. THE OR MUST BE THREE STATEMENTS (`c = ...; c |= ...; c |= ...`), not one
 *    expression. One expression sits at 60/60 lines with the right registers and 31
 *    of 60 matched; splitting it closes the last 29.
 *
 * Lever 2 is the interesting one: a value being assigned TWICE is what keeps it out
 * of local-alloc, and the register it then gets from global-alloc is the one the ROM
 * uses. That is the opposite reflex to the usual "give it its own name".
 */
int UploadBGPalette(unsigned short *src, unsigned short *dst, int amount, int count)
{
    unsigned int c, r, g, b;
    int i;

    if (amount > 0x10000)
        amount = 0x10000;
    for (i = 0; i < count; i++) {
        c = *src++;
        r = c & 0x1f;
        g = c & 0x3e0;
        b = c & 0x7c00;
        r *= amount;
        g *= amount;
        b *= amount;
        c = (r >> 16) & 0x1f;
        c |= (g >> 16) & 0x3e0;
        c |= (b >> 16) & 0x7c00;
        *dst++ = c;
    }
    return 0;
}
