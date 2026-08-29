/* Cluster Func_80c08a8..Func_80c08a8 extracted from goldensun/asm/rom_b5000/rom_bffb8_a_c_a.s.
 *
 * Slotted between rom_bffb8_a_c_a_a.o and the rest of stage1.ld.
 *
 * Allocates a 0x2a0-byte EWRAM block, zeroes it, and clears a field of the
 * iwram_3001f00 record.
 *
 * The iwram pointer is READ BEFORE the DMA and kept in r5, a pushed
 * callee-saved register -- the batch-49 tell that the value has to survive the
 * intervening call, so the load belongs above it in the source.
 *
 * gcc reuses the zero DMA3_CLEAR writes to its stack slot for the `+8` store
 * as well. Nothing in the C asks for that; both are plain zeroes.
 */
#include "dma.h"
extern unsigned char *iwram_3001f00;
extern void *galloc_ewram(int tag, int size);

void Func_80c08a8(void)
{
    void *p;
    unsigned char *q;

    p = galloc_ewram(0xa, 0xa8 << 2);
    q = iwram_3001f00;
    DMA3_CLEAR(p, 0x2a0);
    *(int *)(q + 8) = 0;
}
