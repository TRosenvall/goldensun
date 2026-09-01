#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001e6c[];
extern unsigned int iwram_3001e40;
extern void (*iwram_3001cfc)(void);
extern unsigned char ewram_201c000[];
extern unsigned char gBuffer[65536];
extern void Func_801179c(void);
extern void Func_8011568(void);
extern void Func_80042c8(void *func);
extern void WaitFrames(unsigned int nframes);
extern void Func_8012388(void *dst, void *src);

typedef struct {
    unsigned char pad[0x100];
    unsigned short f100;
    unsigned short f102;
} FieldState;

void Func_8011590(void)
{
    unsigned char **p;
    unsigned char *a;
    FieldState *base;
    unsigned int n;

    p = iwram_3001e6c;
    a = *p++;
    base = (FieldState *)*p;
    base->pad[0xfc] = 1;
    Func_80042c8(Func_801179c);
    DMA3_COPY((void *)0x6004000, ewram_201c000, 0x2000);
    WaitFrames(1);
    n = iwram_3001e40 & 1;
    Func_8012388(a + (n * 5 << 10) + (0xc8 << 4), gBuffer);
    base->f100 = 0xc8;
    base->f102 = 0xff;
    iwram_3001cfc = Func_8011568;
}
