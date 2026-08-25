/* Cluster DeleteSpriteLayer..DeleteSpriteLayer extracted from goldensun/asm/rom_9000/rom_b798_c_c.s.
 *
 * Slotted between rom_b798_c_c_a.o and the rest of stage1.ld.
 *
 * Zeroes a 0x18-byte sprite-layer record, if the pointer is non-null, with
 * DMA3_CLEAR from include/dma.h. One `.global .L12f20` was added to the .s so
 * this could be split out -- fourteenth in this tree, verified byte-neutral
 * before the split.
 */
#include "dma.h"

void DeleteSpriteLayer(void *p)
{
    if (p != 0)
        DMA3_CLEAR(p, 0x18);
}
