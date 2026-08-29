/* OvlFunc_879_20081c0  --  0x020081c0
 *
 * Cut out of goldensun/asm/overlays/rom_779188/ovl_30_c_c_b.s.
 *
 * Loads the cursor sprite: allocate a scratch buffer, claim a sprite slot the
 * first time only, decompress, DMA the palette, upload the tiles, then SPIN
 * until DMA3 is idle before freeing the buffer.
 *
 * `.L650` is the cached slot and lives in the sibling piece, reached through
 * `.global .L650` -- a label emits no bytes, so that export was its own commit
 * with compare green either side.
 *
 * THE DMA3 BUSY-WAIT WANTS `dma[2]` THROUGH A `vu32 *` LOCAL. Both
 * `REG_DMA3CNT` and `(&REG_DMA3SAD)[2]` fold to `ldr r3, =0x40000dc /
 * ldr r3, [r3]`; the ROM has `ldr r1, =REG_DMA3SAD / ldr r3, [r1, #8]`, which
 * is the base held in a local and indexed.
 *
 * Drafted by a parallel screening agent and re-screened here before wiring.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char L650[] __asm__(".L650");
extern int _FILE_1c;

extern void *__Func_8004970(int size);
extern short __AllocSpriteSlot(void);
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern void __free(void *p);

void OvlFunc_879_20081c0(void)
{
    unsigned char *buf;
    vu32 *dma;
    unsigned char *p;

    buf = __Func_8004970(0xa4 << 3);
    if (*(short *)L650 == -1)
        *(short *)L650 = __AllocSpriteSlot();
    __DecompressLZ(__GetFile((int)&_FILE_1c), buf);
    DMA3_COPY(buf, (void *)0x50003e0, 0x20);
    p = buf + 0x20;
    __UploadSpriteGFX(*(short *)L650, 0xa0 << 3, p);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    __free(buf);
}
