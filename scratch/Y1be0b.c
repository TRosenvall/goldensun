#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned char Func_8015afc[];
extern unsigned char gPtrs[];

void DecompressIcon(char *rec)
{
    int size;
    void *buf;
    void (*fp)(void *, char *);

    size = 0x278;
    buf = galloc_iwram(0x31, size);
    DMA3_COPY(Func_8015afc, buf, size);
    fp = *(void (**)(void *, char *))(gPtrs + 0xc4);
    fp(*(void **)(rec + 0x604), rec);
    gfree(0x31);
}
