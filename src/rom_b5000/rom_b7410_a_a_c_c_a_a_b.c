/* Func_80b78e4  --  0x080b78e4, split out of asm/rom_b5000/rom_b7410_a_a_c_c_a_a.s;
 * Func_80b7738 stays in _a.s and Func_80b7994 in _c.s. Matched from scratch.
 *
 * Returns `int` with no return statement: the ROM's `pop {r1}` epilogue names a
 * return value, and `void` gives `pop {r0}` (the batch-285 tell).
 */
extern unsigned char *_GetUnit(int id);

int Func_80b78e4(int id, unsigned char *out)
{
    unsigned char *u;
    int f;
    signed char s;

    u = _GetUnit(id);
    s = u[0x131];
    f = 0;
    if (s == 1)
        f = 1;
    if (s == 2)
        f |= 2;
    if (u[0x138] != 0)
        f |= 0x20;
    if (u[0x13b] != 0) {
        f |= 4;
        if (u[0x128] == 0x79 || u[0x128] == 0x94)
            f &= ~4;
    }
    if (u[0x13d] != 0)
        f |= 8;
    if (u[0x140] != 0)
        f |= 0x40;
    if (u[0x13c] != 0)
        f |= 0x10;
    if (u[0x141] != 0)
        f |= 1 << (u[0x141] + 6);
    *(short *)(out + 0x1c) = f;
}
