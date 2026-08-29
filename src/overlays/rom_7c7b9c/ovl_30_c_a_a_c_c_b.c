/* Cluster OvlFunc_943_200b9b8..OvlFunc_943_200b9b8 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c.s.
 *
 * Slotted between ovl_30_c_a_a_c_c_a.o and the rest of the overlay.
 *
 * SHARED STACK-ARG VALUE, held in r5 across both calls -- a callee-saved
 * register the prologue pushes, which is the tell that the value has to survive
 * the first `bl`. One local passed as the fifth argument of both reproduces it,
 * and the store order falls out of that with no reordering of the C.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_943_200b9b8(void)
{
    int m;
    int n;

    m = 5;
    n = 4;
    __CopyMapTiles(0x42, 0x3d, 0x40, 0x28, m, n);
    n = 0x27;
    __Func_8010704(0, 0, 5, 4, m, n);
}
