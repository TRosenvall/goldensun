/* Cluster OvlFunc_883_200d928..OvlFunc_883_200d928 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_780898/ovl_30_c_c_c_c_c_a_a_a.o and the rest of the overlay
 * in goldensun/overlays/rom_780898/overlay.ld.
 *
 * One map edit, then two local calls.
 *
 * THE FIRST FUNCTION FOUND BY tools/match_shapes.py --near. Its skeleton
 * differs from src/overlays/rom_7a6ae4/ovl_30_c_a_c_c_b.c in exactly one line:
 * where that one ends with `__SetFlag(0x305)` this one makes two argument-less
 * calls. Everything the exemplar carries -- the stack-arg-pair naming, the
 * assignment order, the six-argument signature -- transferred unchanged, which
 * is the case for loosening the matcher at all.
 *
 * The exemplar's own body is worth ignoring as prose: it is upstream code with
 * `new_var` names and redundant temporaries, and only its SHAPE was used here.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_883_20080c4(void);
extern void OvlFunc_883_200d950(void);

void OvlFunc_883_200d928(void)
{
    int m;
    int n;

    m = 0x16;
    n = 0x24;
    __Func_8010704(0x11, 0, 3, 1, m, n);
    OvlFunc_883_20080c4();
    OvlFunc_883_200d950();
}
