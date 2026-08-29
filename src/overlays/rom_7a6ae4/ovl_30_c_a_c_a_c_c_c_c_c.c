/* OvlFunc_920_20081bc extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Near-twin of ovl_30_c_a_c_a_c_c_c_c_b.c immediately above.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_920_20081bc(void)
{
    int m;
    int n;

    m = 1;
    n = 3;
    __CopyMapTiles(0x5f, 0x15, 0x61, 0x15, m, n);
    m = 0x20;
    n = 0x19;
    __Func_8010704(0x2e, 0x26, 3, 1, m, n);
}
