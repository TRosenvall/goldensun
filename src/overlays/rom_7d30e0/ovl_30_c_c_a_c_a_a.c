/* OvlFunc_948_2009c6c extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * SHARED STACK-ARG VALUE, held in r5 across both calls -- a callee-saved
 * register the prologue pushes, which is the tell that the value has to survive
 * the first `bl`. One local passed as the fifth argument of both reproduces it,
 * and the store order falls out of that with no reordering of the C.
 *
 * HERE IT IS THE SECOND SLOT THAT IS SHARED, not the first: r5 goes to
 * [sp, #4] and the per-call value to [sp]. Which local is shared is read off
 * which stack slot the saved register feeds, and the two orders otherwise look
 * identical.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2009c6c(void)
{
    int m;
    int n;

    n = 0x37;
    m = 0x26;
    __Func_8010704(0x26, 0x38, 1, 1, m, n);
    m = 0x2a;
    __Func_8010704(0x2a, 0x38, 1, 1, m, n);
}
