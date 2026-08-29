/* Cluster OvlFunc_901_2008e30..OvlFunc_901_2008e30 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_c_a_a.s.
 *
 * Slotted between ovl_314_c_c_a_c_a_a_a.o and the rest of the overlay.
 *
 * TWO STACK-ARG PAIRS. The second one stores the SAME register into both
 * slots (`mov r3,#3 / str r3,[sp] / str r3,[sp,#4]`), which is the shared-value
 * case -- but it is spelled here as two plain literals, not as a named local.
 *
 * REUSING THE FIRST PAIR`S LOCAL FOR IT IS WRONG. `n = 3;` then passing `n, n`
 * perturbs the FIRST pair`s register assignment three instructions earlier and
 * comes out 3 of 22. A fresh local works and so do bare literals; the literals
 * are kept, being the shorter of the two that match.
 *
 * Near-twin of ovl_314_c_c_a_c_a_a_c.c, differing in two tile coordinates.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_901_2008e30(void)
{
    int m;
    int n;

    m = 0xd;
    n = 0x19;
    __Func_8010704(0x29, 0x2b, 1, 1, m, n);
    __CopyMapTiles(0x28, 0x2a, 0xc, 0x16, 3, 3);
}
