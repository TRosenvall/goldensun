/* Cluster OvlFunc_939_200918c..OvlFunc_939_200918c extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_c_a_c.s.
 *
 * Slotted between ovl_314_c_a_c_a.o and the rest of the overlay.
 *
 * Near-twin of ovl_314_c_a_b.c (OvlFunc_939_2008fa0, batch 57): three calls
 * sharing the SECOND stack slot in r5, differing only in the first two register
 * arguments and the three [sp] values.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_939_200918c(void)
{
    int m;
    int n;

    n = 0xb;
    m = 7;
    __Func_8010704(7, 0xc, 1, 1, m, n);
    m = 8;
    __Func_8010704(7, 0xc, 1, 1, m, n);
    m = 9;
    __Func_8010704(7, 0xc, 1, 1, m, n);
}
