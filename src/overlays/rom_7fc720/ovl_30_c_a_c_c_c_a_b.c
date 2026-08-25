/* Cluster OvlFunc_973_20086f8..OvlFunc_973_20086f8 extracted from goldensun/asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a.s.
 *
 * Slotted between ovl_30_c_a_c_c_c_a_a.o and the rest of the overlay.
 *
 * INSTRUCTION-FOR-INSTRUCTION IDENTICAL to
 * src/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_b.c in another overlay -- same
 * offsets, same constants, same callee. Read that file for why the offsets are
 * plain literals.
 */
extern unsigned char *iwram_3001ebc;
extern void __Func_8091e9c(int a);

void OvlFunc_973_20086f8(int a)
{
    unsigned char *p;

    p = iwram_3001ebc;
    *(int *)(p + 0x1c0) = 0x201;
    *(int *)(p + 0x1c8) = 0x18;
    __Func_8091e9c(a);
}
