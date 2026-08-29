/* Cluster OvlFunc_901_2008d24..OvlFunc_901_2008d24 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_c.s.
 *
 * Preserves the original ROM layout in goldensun/overlays/rom_797990/overlay.ld.
 *
 * Byte-identical to src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_b.c -- see
 * src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c for the lever. */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_901_2008d24(void)
{
    int m;
    int n;

    __SetFlag(0x80 << 2);
    m = 0x17;
    n = 0x1a;
    __Func_8010704(0x37, n, 4, 2, m, n);
}
