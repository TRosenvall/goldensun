/* OvlFunc_886_2008088, the whole of goldensun/asm/overlays/rom_786f0c/ovl_30_a_a_c.s.
 *
 * Total .text for this TU = 62 bytes (= 0x3e).
 * The .s is replaced outright, so no linker-script change was needed.
 *
 * One per-frame integration step for a projectile-ish entity: the two 8.8
 * velocity shorts at +0x64 and +0x66 are scaled up by 256 and added to the
 * position words at +8 and +0xc, a fixed 0x666 is added to both angle words at
 * +0x18 and +0x1c, and then the two velocity shorts are themselves stepped --
 * one up by five, one down by one. Returns 0 unconditionally.
 *
 * TWO BYTE-IDENTICAL COPIES exist -- OvlFunc_886_2008088 (rom_786f0c) and
 * OvlFunc_956_200937c (rom_7e0928) -- sharing this C verbatim.
 *
 * THE VELOCITY FIELDS ARE `short`, READ BOTH WAYS, AND THAT IS NOT A CONFLICT.
 * The ROM reads them with `ldrsh` where the value is shifted and added into a
 * word, and with `ldrh` where it is stepped and stored straight back. Declaring
 * them `short` gives both: gcc uses the unsigned load for the step because the
 * sign cannot affect an add that is truncated back to sixteen bits. Declaring
 * them `unsigned short` to match the `ldrh` would break the `ldrsh`.
 *
 * Nothing else was needed -- exact on the first screen.
 */

struct E {
    unsigned char pad00[8];
    int x;
    int y;
    unsigned char pad10[8];
    int a;
    int b;
    unsigned char pad20[0x44];
    short f64;
    short f66;
};

int OvlFunc_886_2008088(struct E *e)
{
    e->x += e->f64 << 8;
    e->y += e->f66 << 8;
    e->a += 0x666;
    e->b += 0x666;
    e->f64 += 5;
    e->f66 -= 1;
    return 0;
}
