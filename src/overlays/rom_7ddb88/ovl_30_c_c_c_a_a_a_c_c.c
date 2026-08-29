/* Cluster OvlFunc_955_20082c0..OvlFunc_955_20082c0 extracted from goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Stack-arg-pair. Note 0x20 appears as both the first argument and the [sp]
 * value and is NOT shared: the ROM builds r0 fresh with its own `mov`, so the
 * argument stays a literal and only the stack value is named.
 */
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_20082c0(void)
{
    int m;
    int n;

    __SetFlag(0x333);
    m = 0x20;
    n = 0x4d;
    __Func_8010704(0x20, 0x25, 1, 4, m, n);
}
