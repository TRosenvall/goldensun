/* Cluster OvlFunc_930_2009028..OvlFunc_930_2009028 extracted from goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_a_c_c_c_c_c_b.o and the rest of the overlay.
 *
 * Near-twin of OvlFunc_930_2008ff0 in the .o immediately above, differing only
 * in the first argument (6 against 5). See that file for why this pair is
 * built at -O2 and what the Makefile was doing to it.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);

void OvlFunc_930_2009028(void)
{
    int m;
    int n;

    m = 1;
    n = 2;
    __CopyMapTiles(6, 0x1c, 5, 0xd, m, n);
    m = 5;
    n = 0xd;
    __Func_8010704(6, 0x1c, 1, 2, m, n);
    __CutsceneWait(1);
}
