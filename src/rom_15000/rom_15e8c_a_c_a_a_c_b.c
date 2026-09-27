/* ClearUIRegion  --  0x08016178, split out of asm/rom_15000/rom_15e8c_a_c_a_a_c.s;
 * Func_8016018 and Func_80160fc stay in _a.s. Matched from scratch.
 *
 * - The base must be an INTEGER, not a `char *`, and the address written
 *   `(y*32 + x)*2 + base`: only that gives the ROM's `add r5,r3,r2` order.
 * - A .greg line "Register N now in 12" means RELOAD evicted a pseudo: here the
 *   loop stride lost r1 because reload needed a low register for `y + i` (y
 *   lives in r8). Writing `if (y+i > 0x10) fill = A; else fill = B;` instead of
 *   set-then-override frees fill's register at that point, so reload takes it
 *   instead. 73 -> 1.
 */
extern unsigned int iwram_3001e8c;
extern void Func_801e260(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

void ClearUIRegion(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    unsigned int base = iwram_3001e8c;
    unsigned short *p = (unsigned short *)((y * 32 + x) * 2 + base);
    unsigned int fill = 0xf000;
    unsigned int i, j;

    if (y + h > 0x14)
        h = 0x14 - y;
    if (w < 2)
        w = 2;
    if (w > 0x1e)
        w = 0x1e;
    if (h < 2)
        h = 2;
    if (h > 0x1e)
        h = 0x1e;
    Func_801e260(x, y, w, h);
    for (i = 0; i < h; i++) {
        if (*(unsigned char *)(base + 0xea5) != 0) {
            if (y + i > 0x10)
                fill = 0xf07f;
            else
                fill = 0xf000;
        }
        for (j = 0; j < w; j++)
            *p++ = fill;
        p += 32 - w;
    }
    *(unsigned char *)(base + 0xea3) = 1;
}
