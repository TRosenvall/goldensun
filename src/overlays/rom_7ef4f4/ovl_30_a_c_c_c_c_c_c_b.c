/* Cluster OvlFunc_965_200a4b0..OvlFunc_965_200a4b0 extracted from goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_a_c_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * Stack-arg pair named as two locals, stored before the call. The callee IS
 * declared here -- the ROM writes r0 FIRST -- which is the additive side of
 * the declaration lever, the opposite of the 888/930 family in batches 44-45
 * where r0 comes last and the declaration is withheld.
 *
 * NOTE ON FLAGS: this file inherits -O1 from the wildcard rule
 * `ovl_30_a_c_c_c_c_c%`, which was written for a neighbouring .s -- the trap
 * from batch 45. Checked before splitting rather than after: the function is
 * fourteen instructions of `mov` and `str` and matches byte-for-byte at BOTH
 * -O1 and -O2, so the inherited rule is harmless here and the Makefile is left
 * alone.
 */
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);

void OvlFunc_965_200a4b0(void)
{
    int m;
    int n;

    m = 0x11;
    n = 0x4e;
    __Func_8010788(0x20, 0x4e, 1, 2, m, n);
}
