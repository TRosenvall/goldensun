/* Cluster OvlFunc_936_200958c..OvlFunc_936_200958c extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a.s.
 *
 * Slotted between ovl_30_c_c_c_a_a_c_a_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. The
 * flag id 0x80 << 2 appears on both sides of a call -- once in the `if`
 * condition, once in the body -- and at plain -O2 gcc's second CSE pass hoists
 * it into r5, spending a push, a pop and a move to save one two-instruction
 * rebuild. The ROM builds it twice. 13 of 16 with the flag off, exact with it
 * on. Ninth TU in this tree to need it.
 *
 * THE BASIC-BLOCK LEVER DOES NOT APPLY HERE and was tried first: naming the
 * constant in a block that dominates the call gives 13 of 17 -- one
 * instruction LONGER -- because the named local is what CSE merges. This is
 * the constant-CSE class, not arg-interleave, and the two look alike in the
 * listing.
 */
extern unsigned char *iwram_3001ee0;
extern int __GetFlag(int id);
extern void __ClearFlag(int id);

void OvlFunc_936_200958c(void)
{
    unsigned char *p;

    if (__GetFlag(0x80 << 2)) {
        p = iwram_3001ee0;
        *(int *)(p + 0x18) = 0;
        __ClearFlag(0x80 << 2);
    }
}
