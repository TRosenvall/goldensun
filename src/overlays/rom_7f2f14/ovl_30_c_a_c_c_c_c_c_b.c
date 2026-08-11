/* Cluster OvlFunc_968_200a26c..OvlFunc_968_200a26c extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_c_a.o and asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * Sibling of src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_b.c in the same overlay
 * -- same structure, four constants different, and a different tail call. Read
 * that header for the stack-arg-pair lever both of them need.
 *
 * As there, the value repeated between argument two and argument six (0x27
 * here) is named once and used twice; that is what frees the register the other
 * stack value needs.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_200a26c(void)
{
    int m;
    int n;

    __CutsceneStart();
    if (OvlFunc_968_2008cc8() == 0) {
        m = 0x2c;
        n = 0x27;
        __Func_8010704(0x6c, n, 0xd, 7, m, n);
        OvlFunc_968_2008374();
    }
    __CutsceneEnd();
    OvlFunc_968_2009f60();
}
