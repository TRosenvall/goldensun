/* Cluster OvlFunc_882_200810c..OvlFunc_882_200810c extracted from goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_a_a.s.
 *
 * Preserves the original ROM layout in goldensun/overlays/rom_77dd1c/overlay.ld.
 *
 * UNPARKED BY THE STACK-ARG-PAIR LEVER. This family was parked as
 * "STACK-ARGUMENT REGISTER REUSE": the ROM builds both stack constants into
 * separate registers before storing either, and gcc builds one, stores it, and
 * reuses the register.
 *
 * The park recorded naming the two values as tried and costing an instruction.
 * It does work -- what was missing is that the value appearing BOTH as a
 * register argument and as a stack argument (0x54 here, 0x1a in the other two)
 * has to be named ONCE AND USED TWICE, and the assignments have to sit
 * immediately before the call. See
 * src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_b.c for the full account.
 *
 * Four members, in four different overlays. Two are byte-identical and two
 * differ only in their five constants. */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_882_200810c(void)
{
    int m;
    int n;

    __SetFlag(0x84 << 2);
    m = 0xa;
    n = 0x54;
    __Func_8010704(0x28, n, 7, 4, m, n);
}
