/* Cluster OvlFunc_916_2008f74..OvlFunc_916_2008f74 extracted from goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * Two DMA3 transfers of 0x1c0 bytes from palette RAM into the iwram_3001ed0
 * block, then a call. Uses DMA3_COPY from include/dma.h, the same inherited
 * inline-asm helper the neighbouring solved file
 * src/overlays/rom_7a1ff0/ovl_30_c_c_c_c_b.c already uses -- so this adds no
 * new inline assembly to the tree, only another user of what is there.
 *
 * THE BLOCK POINTER MUST BE NAMED. Written as `iwram_3001ed0` at both call
 * sites, gcc reloads it for the second transfer and builds the second
 * destination with a destructive `add r1, r2`; the ROM loads it once into r4
 * and uses the three-operand `add r1, r4, r2`. 5 of 23 either way it is read.
 * One local fixes both.
 *
 * ONE OF FOUR INSTRUCTION-IDENTICAL COPIES, one per overlay: OvlFunc_914_2008bcc,
 * OvlFunc_915_2008d9c, OvlFunc_916_2008f74, OvlFunc_917_2009878.
 */
#include "dma.h"
extern unsigned int iwram_3001ed0;
extern void __Func_8091220(int a, int b);

void OvlFunc_916_2008f74(void)
{
    unsigned char *p;

    p = (unsigned char *)iwram_3001ed0;
    DMA3_COPY((void *)(0xa0 << 19), p, 0x1c0);
    DMA3_COPY((void *)0x5000200, p + 0x1c0, 0x1c0);
    __Func_8091220(0x80 << 9, 0);
}
