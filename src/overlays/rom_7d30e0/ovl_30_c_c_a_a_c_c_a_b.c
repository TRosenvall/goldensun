/* Cluster OvlFunc_948_2009a70..OvlFunc_948_2009a70 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_a.o and the rest of the overlay
 * in goldensun/overlays/rom_7d30e0/overlay.ld.
 *
 * One map edit, then clear-and-set a byte on actor 8.
 *
 * FOUND BY tools/match_shapes.py --near, two lines off
 * src/overlays/rom_7ef4f4/ovl_30_c_b.c. The stack-arg-pair naming came across
 * unchanged; what differs is the tail, where that one waits and sets a flag and
 * this one writes through an actor pointer.
 *
 * The store is a POINTER WALK -- the ROM does `add r0, #0x23 / strb r3, [r0]`
 * rather than `strb r3, [r0, #0x23]` -- so the offset is folded into the
 * pointer at the assignment, not at the store. Written as
 * `*((unsigned char *)a + 0x23) = 2` gcc uses the indexed form and the function
 * is one instruction short.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void *__MapActor_GetActor(int slot);

void OvlFunc_948_2009a70(void)
{
    unsigned char *q;
    int m;
    int n;

    m = 0x29;
    n = 0x2a;
    __Func_8010704(0x2c, 0x2a, 1, 1, m, n);
    q = (unsigned char *)__MapActor_GetActor(8) + 0x23;
    *q = 2;
}
