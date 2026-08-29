/* Cluster OvlFunc_883_2008d70..OvlFunc_883_2008d70 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a.s.
 *
 * Preserves the original ROM layout in goldensun/overlays/rom_780898/overlay.ld.
 *
 * Byte-identical to src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c -- see that
 * header for the stack-arg-pair lever this family needed. */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_883_2008d70(void)
{
    int m;
    int n;

    __SetFlag(0x84 << 2);
    m = 0xa;
    n = 0x54;
    __Func_8010704(0x28, n, 7, 4, m, n);
}
