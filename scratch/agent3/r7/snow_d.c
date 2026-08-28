#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void *galloc_ewram(int tag, int size);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void (*t)(void), int m);
extern void Task_Snow(void);
extern unsigned char Data_a001e[];
extern unsigned char *iwram_3001e70;

void StartSnow(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *c;
    int *e;
    int *w;
    int a, b;
    int t;
    int zero;
    unsigned int i;
    volatile unsigned short *r;

    p = (unsigned char *)galloc_ewram(0x1d, 0x410);
    c = p + 8;
    DMA3_FILL(p, 0, 0x410);
    q = (unsigned char *)galloc_ewram(0xe, 0x400);
    DecompressLZ1(Data_a001e, q);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0x300, q);
    gfree(0xe);
    i = 0;
loop:
    {
        w = (int *)c;
        zero = 0;
        *w++ = zero;
        e = *(int **)iwram_3001e70;
        *w++ = 0x40000400;
        *w = 0xd400;
        a = e[0];
        b = e[2];
        *(int *)(c + 0xc) = a;
        *(int *)(c + 0x14) = b;
        a >>= 16;
        b >>= 16;
        *(int *)(c + 0x10) = _Func_8011f54(0, a, b) << 16;
        *(unsigned short *)(c + 0x1c) = (i & 0xf) + 1;
        c += 0x20;
    }
    i++;
    if (i < 0x20)
        goto loop;
    r = &REG_BLDCNT;
    {
        int v = 0x3f00;
        *r = v;
    }
    r++;
    *r = 0x1008;
    r++;
    *r = zero;
    StartTask(Task_Snow, 0xc80);
}
