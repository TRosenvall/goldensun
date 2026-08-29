/* OvlFunc_899_200c840  --  0x0200c840
 *
 * Cut out of goldensun/asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a_b.s.
 *
 * Validates a two-byte tile coordinate against four kinds of obstruction and
 * returns 0 only if all four are clear -- 1 for a null pointer, -1 for a hit.
 *
 * The two coordinates are converted from tile units to 12.20 fixed point by
 * `<< 19` plus a half-tile bias, and the biases are built with `mov` + `lsl`
 * (0x90 << 15 and 0x9e << 18) rather than pooled, so they are written that way.
 *
 * Four `if (...) return -1;` guards give the ROM's single shared
 * `mov r0, #1 / neg r0, r0` block -- gcc cross-jumps the four identical
 * returns, which is why there is only one of them in forty-nine instructions.
 *
 * Matched on the first screen.
 */
extern int OvlFunc_899_200c7fc(int x, int y, int n);
extern int OvlFunc_899_200c7bc(int x, int y, int n);

int OvlFunc_899_200c840(unsigned char *p)
{
    int x;
    int y;

    if (p == 0)
        return 1;
    x = (p[0] << 19) + (0x90 << 15);
    y = (p[1] << 19) + (0x9e << 18);
    if (OvlFunc_899_200c7fc(x, y, 0))
        return -1;
    if (OvlFunc_899_200c7bc(x, y, 2))
        return -1;
    if (OvlFunc_899_200c7bc(x, y, 0x18))
        return -1;
    if (OvlFunc_899_200c7bc(x, y, 0x19))
        return -1;
    return 0;
}
