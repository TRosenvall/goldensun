/* OvlFunc_879_2008454  --  0x02008454, split out of
 * asm/overlays/rom_779188/ovl_30_c_c_c.s; the data/bss that followed stay in
 * _c_c.s.
 *
 * Out of a park that sat at a 4-byte size mismatch (9 encodings). Closed by the
 * two edits that closed its sibling OvlFunc_880_2008054 (rom_7795e8): the
 * preceding halfword store written as the typed field `h->f14 = y`, so the
 * byte store's zero is pooled after the call, and one struct-typed declaration
 * of iwram_3001ad0 in place of the __asm__ alias. The pooled 0x1a is
 * _FILE_1a, a __GetFile id (file_table.sym).
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern int _FILE_1a;
extern unsigned char gBuffer[];
struct BgOfs {
    unsigned short h;
    unsigned short v;
};

extern struct BgOfs iwram_3001ad0[];
struct Map {
    unsigned char pad00[0x14];
    unsigned short f14;
};
extern struct Map *iwram_3001e70;
extern void __Func_8003b70(int a);
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);

void OvlFunc_879_2008454(void)
{
    unsigned char *f;
    register unsigned char *b1 __asm__("r1");
    short *m;
    struct BgOfs *q;
    struct Map *h;
    int y;
    int t;
    unsigned int i;
    unsigned int j;
    int w;
    int term;
    int id;

    id = (int)&_FILE_1a;
    __Func_8003b70(0);
    REG_BG2CNT = 0x681;
    iwram_3001ad0[2].v = 0;
    term = 0x1ff;
    f = __GetFile(id);
    DMA3_COPY(f, (void *)(0xa0 << 19), 0x1c0);
    b1 = gBuffer;
    __DecompressLZ(f + (0xe0 << 1), b1);
    DMA3_COPY(gBuffer, (void *)0x6006800, 0x9600);
    m = (short *)0x6003000;
    t = 0xd0 << 1;
    j = 0;
row:
    i = 0;
cell:
    w = t;
    t = ((t << 16) + 0x10000) >> 16;
    *m++ = w;
    i++;
    if (i <= 0x1d)
        goto cell;
    *m++ = term;
    *m++ = term;
    j++;
    if (j <= 0x13)
        goto row;
    q = iwram_3001ad0;
    j = 0;
    do {
        j++;
        q->v = 0;
        q->h = 0;
        q++;
    } while (j <= 3);
    DMA3_COPY(iwram_3001ad0, (void *)REG_ADDR_BG0HOFS, 0x10);
    h = iwram_3001e70;
    y = 0xa0 << 5;
    h->f14 = y;
}
