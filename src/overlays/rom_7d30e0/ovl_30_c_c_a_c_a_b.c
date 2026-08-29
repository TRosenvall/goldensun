/* Cluster OvlFunc_948_2009ca0..OvlFunc_948_2009ca0 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d30e0/ovl_30_c_c_a_c_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7d30e0/overlay.ld.
 *
 * A map edit followed by a byte write on an actor. The fourth, fifth and sixth
 * members of the family headed by
 * src/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_b.c, all found by
 * tools/match_shapes.py and all clean on the first screen.
 *
 * Two things carried across unchanged and are the reason it is a family:
 *
 *   * the two stack arguments are NAMED, in the order the ROM stores them and
 *     immediately before the call -- the stack-arg-pair lever;
 *   * the byte store is a POINTER WALK. The ROM does `add r0, #0x55 / strb`,
 *     not `strb r3, [r0, #0x55]`, so the offset is folded into the pointer at
 *     the assignment rather than at the store. Written indexed, the function is
 *     one instruction short.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void *__MapActor_GetActor(int slot);

void OvlFunc_948_2009ca0(void)
{
    unsigned char *q;
    int m;
    int n;

    m = 0x26;
    n = 0x37;
    __Func_8010704(0x28, 0x36, 1, 1, m, n);
    q = (unsigned char *)__MapActor_GetActor(0xa) + 0x55;
    *q = 0;
}
