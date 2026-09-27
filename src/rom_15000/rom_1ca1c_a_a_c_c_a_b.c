/* SetUIColor  --  0x0801ccc0, split out of asm/rom_15000/rom_1ca1c_a_a_c_c_a.s;
 * Func_801cc50 stays in _a.s. Matched from scratch.
 *
 * `short b7 = b - 7;` as its own local took it from 28 differing to 13; the last
 * 13 were the order of the three c[] stores -- c[0], c[1], c[2] matches, any
 * order starting with c[2] stays at 13. .L36750 is exported by rom_1ca1c_c_c_c.s.
 */
extern unsigned char L36750[] __asm__(".L36750");
extern int Func_801cc50(short *p, int a, int b, int c);

void SetUIColor(int a, int b)
{
    short c[3];
    short h;
    short b7;
    short r, g, bl;
    unsigned char *t;

    h = (a + 0xc) % 0x18 * 4;
    t = L36750;
    b7 = b - 7;
    r = t[(short)(h % 0x60)] + b7;
    g = t[(h + 0x20) % 0x60] + b7;
    bl = t[(h + 0x40) % 0x60] + b7;
    if (r < 0)
        r = 0;
    if (r > 0x1f)
        r = 0x1f;
    if (g < 0)
        g = 0;
    if (g > 0x1f)
        g = 0x1f;
    if (bl < 0)
        bl = 0;
    if (bl > 0x1f)
        bl = 0x1f;
    c[0] = r;
    c[1] = g;
    c[2] = bl;
    *(volatile unsigned short *)0x50001e8 = Func_801cc50(c, 0xeeee, 0xcccc, 0x11110);
    *(volatile unsigned short *)0x50001ea = Func_801cc50(c, 0xd555, 0xbbbb, 0xeeee);
    *(volatile unsigned short *)0x50001ec = Func_801cc50(c, 0xbbbb, 0xaaaa, 0xcccc);
    *(volatile unsigned short *)0x50001ee = Func_801cc50(c, 0xa221, 0x9999, 0xaaaa);
    *(volatile unsigned short *)0x50001f0 = Func_801cc50(c, 0x10888, 0xdddd, 0x13333);
    *(volatile unsigned short *)0x50001f2 = Func_801cc50(c, 0x12221, 0xeeee, 0x15555);
    *(volatile unsigned short *)0x50001f4 = Func_801cc50(c, 0x13bbb, 0x10000, 0x17777);
}
