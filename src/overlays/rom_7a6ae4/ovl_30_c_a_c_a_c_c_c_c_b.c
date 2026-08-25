/* Cluster OvlFunc_920_2008188..OvlFunc_920_2008188 extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_a_c_a_c_c_c_c_a.o and the rest of the overlay.
 *
 * Two stack-arg pairs, each named as its own two locals and stored before its
 * call. Both callees declared -- the ROM writes r0 first for both.
 *
 * Near-twin of ovl_30_c_a_c_a_c_c_c_c_c.c.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_920_2008188(void)
{
    int m;
    int n;

    m = 1;
    n = 3;
    __CopyMapTiles(0x6f, 0x25, 0x61, 0x15, m, n);
    m = 0x20;
    n = 0x18;
    __Func_8010704(0x2e, 0x26, 3, 2, m, n);
}
