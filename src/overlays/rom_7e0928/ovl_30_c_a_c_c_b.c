/* OvlFunc_956_2008714  --  0x02008714
 *
 * Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_c_a_c_c.s.
 *
 * Probes four points around a position -- two below it and two above -- and
 * returns -1 if any of them is occupied. The four offsets are 12.20 fixed point:
 * -0x180000, -0x80000, +0x80000, +0x180000, so half a tile and one and a half
 * tiles either way.
 *
 * The two negative offsets are pooled (`ldr r3, =0xffe80000`) and the two
 * positive ones built with `mov` + `lsl`, which is just what each value costs;
 * writing all four as plain constants reproduces it. Four `if (...) return -1;`
 * statements give the ROM's shared `.L754` tail, because gcc cross-jumps the
 * four identical returns into one.
 *
 * Matched on the first screen.
 */
extern int OvlFunc_956_20086a4(int a, int b);

int OvlFunc_956_2008714(int a, int b)
{
    if (OvlFunc_956_20086a4(a, b + 0xffe80000))
        return -1;
    if (OvlFunc_956_20086a4(a, b + 0xfff80000))
        return -1;
    if (OvlFunc_956_20086a4(a, b + (0x80 << 12)))
        return -1;
    if (OvlFunc_956_20086a4(a, b + (0xc0 << 13)))
        return -1;
    return 0;
}
