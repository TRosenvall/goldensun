/* OvlFunc_880_2008054  --  0x02008054, was asm/overlays/rom_7795e8/ovl_30_c_c_a_a_a_a.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * - The id passed to __GetFile is the symbol _FILE_1a, added to file_table.sym
 *   by value: the ROM POOLS 0x1a, and a bare literal compiles to a `mov`.
 * - A POOLED BYTE-STORE ZERO AFTER A CALL. The ROM loads `actor->f55 = 0`'s zero
 *   from the pool (the "halfword store poisons a later narrow constant"
 *   effect). What makes it is a halfword-zero pseudo in the same extended basic
 *   block: writing the preceding store as the typed field `h->f14 = y` expands
 *   through an AND against a reg:HI 0, cse reuses that register for the byte
 *   store, and reload rematerialises it from the pool. The loop's own halfword
 *   zeros do not reach the tail through a real do/for/while.
 * - One struct-typed declaration of iwram_3001ad0, not an __asm__ alias, also
 *   removed a duplicated pool word.
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
extern unsigned char gState[];
struct Actor {
    unsigned char pad00[0x55];
    unsigned char f55;
};
extern struct Actor *__MapActor_GetActor(int id);

void OvlFunc_880_2008054(void)
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
    unsigned char *g;

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
    g = gState;
    __MapActor_GetActor(*(int *)(g + 0x1f4))->f55 = 0;
}
