/* Cluster OvlFunc_888_200b270..OvlFunc_888_200b270 extracted from goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c.s.
 *
 * Slotted between ovl_30_c_c_a_a_a_c_c_a.o and the rest of the overlay.
 *
 * TWO STACK-ARG PAIRS IN ONE FUNCTION, each named and stored before its call.
 * The two pairs reuse the SAME two locals: the ROM builds the second pair into
 * the same registers, and separate locals for the second call cost two
 * positions.
 *
 * __Func_8010704 is deliberately NOT declared -- its r0 is written LAST, which
 * is the subtractive side of the declaration lever. __CopyMapTiles and
 * __WaitFrames both have r0 first and are declared.
 *
 * tryc.py warns that the reference file keeps a literal pool inside a function;
 * that pool belongs to a SIBLING in the same .s. This function has no pool
 * loads at all -- every constant is an eight-bit `mov` -- so nothing here is
 * PC-relative. Confirmed by make compare.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);

void OvlFunc_888_200b270(void)
{
    int m;
    int n;

    m = 3;
    n = 2;
    __CopyMapTiles(0, 0x40, 0xb, 0x44, m, n);
    m = 0xb;
    n = 8;
    __Func_8010704(0xb, 0xa, 3, 2, m, n);
    __WaitFrames(1);
}
