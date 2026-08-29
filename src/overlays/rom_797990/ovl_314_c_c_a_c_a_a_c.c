/* OvlFunc_901_2008e60 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_c_a_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Near-twin of ovl_314_c_c_a_c_a_a_b.c; see that file for why the second
 * pair is written as literals.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_901_2008e60(void)
{
    int m;
    int n;

    m = 0xd;
    n = 0x19;
    __Func_8010704(0x25, 0x2b, 1, 1, m, n);
    __CopyMapTiles(0x24, 0x2a, 0xc, 0x16, 3, 3);
}
