/* Cluster OvlFunc_919_200826c..OvlFunc_919_200826c extracted from goldensun/asm/overlays/rom_7a67d8/ovl_30_c_a_c.s.
 *
 * Total .text for this TU = 48 bytes (= 0x30).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a67d8/ovl_30_c_a_c_a.o and
 * asm/overlays/rom_7a67d8/ovl_30_c_a_c_c.o in
 * goldensun/overlays/rom_7a67d8/overlay.ld.
 *
 * Picks one of two scroll values by a VCOUNT threshold and writes it to
 * BG3HOFS. A LEAF FUNCTION -- no calls at all, which is why it was reachable:
 * no callee signature to guess and no argument setup to get wrong.
 *
 * Two spellings, both needed and both about ORDER rather than content:
 *
 *   - The loaded value goes through a named local before the store.  Written as
 *     `*(unsigned short *)REG_ADDR_BG3HOFS = *s;` gcc materialises the
 *     DESTINATION address first; the ROM loads the value first
 *     (`ldrh r2, [r3] / ldr r3, =0x400001c / strh r2, [r3]`).  Naming the value
 *     forces that order.
 *
 *   - The two arms are written in the ROM's order, `if (v >= lim) ... else ...`.
 *     Written the other way round gcc inverts the test and emits `bge` where
 *     the ROM has `blt`.  Same instruction count, opposite branch sense -- the
 *     last remaining difference, and worth knowing that gcc does NOT
 *     canonicalise this: the source's arm order survives into the branch.
 */
#include "gba/io.h"

extern unsigned char L610[] __asm__(".L610");
extern unsigned char L614[] __asm__(".L614");
extern unsigned char L616[] __asm__(".L616");

void OvlFunc_919_200826c(void)
{
    unsigned short *s;
    int v;
    int lim;
    int h;

    v = REG_VCOUNT;
    lim = *(int *)L610;
    if (v >= lim)
        s = (unsigned short *)L614;
    else
        s = (unsigned short *)L616;
    h = *s;
    *(unsigned short *)REG_ADDR_BG3HOFS = h;
}
