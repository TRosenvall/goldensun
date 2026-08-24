/* Cluster OvlFunc_883_2009280..OvlFunc_883_2009280 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_c_a.o and the rest of
 * the overlay in goldensun/overlays/rom_780898/overlay.ld.
 *
 * The undo of src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_b.c: the
 * same three operations on the same flag and the same map rectangle, in the
 * opposite order, with __ClearFlag where that one has __SetFlag.
 *
 * Same two levers, and the ORDER OF THE STATEMENTS is what places them: the
 * stack-arg pair is named immediately before its call, which here is at the END
 * of the function rather than the start. Adjacency is to the call, not to a
 * position in the body.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __ClearFlag(int id);

void OvlFunc_883_2009280(void)
{
    int m;
    int n;

    __CutsceneStart();
    OvlFunc_883_200b380(0, 0xd, 0xa, 1);
    __ClearFlag(0x81 << 2);
    m = 0x14;
    n = 0x32;
    __Func_8010704(0x31, 0x2e, 8, 4, m, n);
    __CutsceneEnd();
}
