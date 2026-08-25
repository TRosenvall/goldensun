/* Cluster OvlFunc_959_200a2a0..OvlFunc_959_200a2a0 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c.s.
 *
 * Slotted between ..._c_a.o and the rest of the overlay.
 *
 * SHARED STACK-ARG VALUE, held in r5 across both calls -- a callee-saved
 * register the prologue pushes, which is the tell that the value has to survive
 * the first `bl`. One local passed as the fifth argument of both reproduces it,
 * and the store order falls out of that with no reordering of the C.
 *
 * One of three near-twins in this overlay with OvlFunc_959_200a26c and
 * OvlFunc_959_200a2d4, differing only in constants.
 */
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_200a2a0(void)
{
    int m;
    int n;

    m = 6;
    n = 0x37;
    __Func_80105d4(2, 0x54, 1, 2, m, n);
    n = 0xa;
    __Func_8010704(5, 9, 1, 1, m, n);
}
