/* Cluster OvlFunc_948_2009da0..OvlFunc_948_2009da0 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_a.s.
 *
 * Slotted between ovl_30_c_c_c_a_a.o and the rest of the overlay.
 *
 * Four calls with identical register arguments, sharing the SECOND stack slot
 * in r5 and varying only the first (0x31 through 0x34).
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2009da0(void)
{
    int m;
    int n;

    n = 0x37;
    m = 0x31;
    __Func_8010704(0x30, 0x37, 1, 1, m, n);
    m = 0x32;
    __Func_8010704(0x30, 0x37, 1, 1, m, n);
    m = 0x33;
    __Func_8010704(0x30, 0x37, 1, 1, m, n);
    m = 0x34;
    __Func_8010704(0x30, 0x37, 1, 1, m, n);
}
