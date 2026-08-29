/* Cluster OvlFunc_920_2008148..OvlFunc_920_2008148 extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c.s.
 *
 * Slotted between ovl_30_c_a_c_a_c_c_a.o and the rest of the overlay.
 *
 * Stack-arg pair named as two locals, stored before the call. The callee IS
 * declared here -- the ROM writes r0 FIRST -- which is the additive side of
 * the declaration lever, the opposite of the 888/930 family in batches 44-45
 * where r0 comes last and the declaration is withheld.
 *
 * Near-twin of ovl_30_c_a_c_a_c_c_c_b.c next door, differing only in the
 * first argument (1 against 0).
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_920_2008148(void)
{
    int m;
    int n;

    m = 0x15;
    n = 0xe;
    __Func_8010704(1, 0, 1, 1, m, n);
}
