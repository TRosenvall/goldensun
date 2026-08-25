/* Cluster OvlFunc_959_200a26c..OvlFunc_959_200a26c extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c.s.
 *
 * Slotted between ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * A STACK-ARG VALUE SHARED ACROSS BOTH CALLS, and the ROM says so by putting it
 * in r5 -- a callee-saved register, kept live across the first `bl` -- and
 * pushing r5 in the prologue. That is the tell: a value in a saved register
 * around a call is one the source uses on both sides of it.
 *
 * The store order differs from the other members of this family. Here [sp, #4]
 * is written FIRST, early in the argument block, and [sp] last, immediately
 * before the `bl`. That falls out of `m` being the shared one; no reordering of
 * the C was needed.
 */
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_200a26c(void)
{
    int m;
    int n;

    m = 0x15;
    n = 0x51;
    __Func_80105d4(2, 0x52, 1, 2, m, n);
    n = 0x22;
    __Func_8010704(0x15, 0x20, 1, 1, m, n);
}
