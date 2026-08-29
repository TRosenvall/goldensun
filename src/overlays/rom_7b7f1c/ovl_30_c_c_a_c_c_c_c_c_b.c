/* Cluster OvlFunc_930_2008ff0..OvlFunc_930_2008ff0 extracted from goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_a_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * Near-twin of OvlFunc_930_2009028 next door (differing only in the first
 * argument, 5 against 6) and of OvlFunc_888_200b270 in batch 44: two
 * stack-arg pairs reusing the same two locals, with the second callee left
 * undeclared because its r0 is written last.
 *
 * THIS MATCHES AT -O2 AND FAILS AT -O1, and the Makefile was silently giving
 * it -O1. The rule `ovl_30_c_c_a_c_c_c_c%` was written for a different .s
 * further down the same split chain and captured this one by prefix. At -O1
 * the diff is four lines of argument fill order -- r0 emitted first where the
 * ROM emits it last -- which is indistinguishable from the real fill-order
 * blocker. Four hypotheses about the C were tested and refuted before the
 * flag was suspected; see the narrowed rule in the Makefile.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);

void OvlFunc_930_2008ff0(void)
{
    int m;
    int n;

    m = 1;
    n = 2;
    __CopyMapTiles(5, 0x1c, 5, 0xd, m, n);
    m = 5;
    n = 0xd;
    __Func_8010704(5, 0x1c, 1, 2, m, n);
    __CutsceneWait(1);
}
