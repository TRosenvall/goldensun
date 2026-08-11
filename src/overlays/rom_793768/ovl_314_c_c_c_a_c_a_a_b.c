/* Cluster OvlFunc_898_2008ea4..OvlFunc_898_2008ea4 extracted from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a.s.
 *
 * Preserves the original ROM layout in goldensun/overlays/rom_793768/overlay.ld.
 *
 * Same family as src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c, differing only in
 * its five constants. See that header for the stack-arg-pair lever. */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_898_2008ea4(void)
{
    int m;
    int n;

    __SetFlag(0x80 << 2);
    m = 0x17;
    n = 0x1a;
    __Func_8010704(0x37, n, 4, 2, m, n);
}
