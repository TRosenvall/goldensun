/* Cluster OvlFunc_970_2008f30..OvlFunc_970_2008f30 extracted from
 * goldensun/asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c.s.
 *
 * Total .text for this TU = 54 bytes (= 0x36) plus its literal pool.
 * Slotted between the _a and _c pieces in goldensun/overlays/rom_7fa4ec/overlay.ld.
 *
 * Kicks off a DMA0 raster effect. Picks a 0x780-byte scanline table by the
 * index byte at [iwram_3001ed8]+0xf00, writes its first word straight to
 * REG_BG3HOFS -- a 32-bit store, so it sets HOFS and VOFS together -- and hands
 * the rest to DMA0 with the HBlank-repeat control word 0xa6600001.
 *
 * THE ORDER OF TWO STATEMENTS DECIDES A REGISTER. `dst` has to be assigned
 * BEFORE the prefix call, not after. Assigned after, gcc has r1 free during
 * UnknownDMAPrefix() and uses it as the read-modify-write scratch; the ROM uses
 * r4. Assigning `dst` first makes r1 spoken for and pushes the scratch to r4 --
 * which is a free choice for gcc because GCC296_CFLAGS carries -fcall-used-r4.
 * The `ldr r1, =REG_BG3HOFS` still lands in the middle of the prefix either
 * way; only the scratch register moves. That was the whole 4-line difference.
 *
 * DMA0_SET was added to include/dma.h for this. It is DMA3_SET with
 * &REG_DMA0SAD as its base, which matters because UnknownDMAPrefix() has
 * already loaded that address -- gcc CSEs the two and the function loads it
 * once, as the ROM does.
 */

#include "dma.h"

extern unsigned char *iwram_3001ed8;

void OvlFunc_970_2008f30(void)
{
    unsigned char *base;
    unsigned int *src;
    int idx;
    vu32 *dst;

    base = iwram_3001ed8;
    idx = base[0xf00];
    src = (unsigned int *)(base + idx * 0x780);
    dst = (vu32 *)&REG_BG3HOFS;
    (void) UnknownDMAPrefix();
    *dst = *src++;
    DMA0_SET(src, (void *)dst, 0xa6600001);
}
