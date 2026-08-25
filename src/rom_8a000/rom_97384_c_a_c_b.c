/* Cluster Func_8097868..Func_8097868 extracted from goldensun/asm/rom_8a000/rom_97384_c_a_c.s.
 *
 * Total .text for this TU = 70 bytes (= 0x46).
 * Placed in the run in goldensun/stage1.ld.
 *
 * Arms a DMA0 scroll effect from a per-map table of 81-word records, but only
 * while the flag byte at +0x294 is clear. Same shape as OvlFunc_970_2008f30 in
 * batch 70, using the same DMA0_SET and UnknownDMAPrefix from include/dma.h.
 *
 * A NAMED OFFSET FORCES THE THREE-OPERAND ADD. The ROM computes the record
 * address with `add r0, r4, r0` -- destination, base, index -- where gcc emits
 * the two-operand `add r0, r4`. Both write r0 and both are legal; gcc picks the
 * short form whenever the destination is also an operand.
 *
 * Written `(char *)b + i * 4` inline it takes the short form. Giving the scaled
 * offset its own statement --
 *
 *      off = i * 4;
 *      src = (char *)b + off;
 *
 * -- produces the ROM's three-operand form. Scaling into `i` itself
 * (`i = i * 4;` then `b + i`) works equally well; what matters is that the
 * addition is a statement of its own rather than a subexpression of the call
 * argument.
 *
 * NOTE: the reference keeps its pool inside the function, so the clean screen
 * here was unproven until `make compare` agreed.
 */

#include "dma.h"

typedef struct {
    unsigned char pad[0x28a];
    unsigned char f28a;
    unsigned char pad28b[9];
    unsigned char f294;
} Blk;

extern Blk *iwram_3001ea8;

void Func_8097868(void)
{
    Blk *b;
    int i;
    int off;
    char *src;
    vu32 *dst;

    b = iwram_3001ea8;
    if (b->f294 != 0)
        return;
    i = b->f28a;
    i = i * 81;
    dst = (vu32 *)&REG_BG0HOFS;
    (void) UnknownDMAPrefix();
    off = i * 4;
    src = (char *)b + off;
    DMA0_SET(src, (void *)dst, 0xa2600001);
}
