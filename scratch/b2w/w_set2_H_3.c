#include "dma.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x1c - 0x18];
    short f1c;
    unsigned char pad1e[0x20 - 0x1e];
};

extern int **iwram_3001e70;
extern unsigned char Data_a001e[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void *f, int pri);
extern void Task_Snow(void);

void StartSnow(void)
{
    unsigned char *p;
    unsigned char *g;
    struct Ent *e;
    unsigned int i;
    int t;
    int *w;
    int *q;
    int x;
    int y;
    int c1;
    int c2;
    unsigned int zero;
    p = galloc_ewram(0x1d, 0x82 << 3);
    e = (struct Ent *)(p + 8);
    zero = 0;
    DMA3_SET(&zero, p, 0x85000000 | ((0x82 << 3) / 4));
    g = galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_a001e, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0xc0 << 2, g);
    gfree(0xe);
    i = 0;
    do {
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        w = *iwram_3001e70;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        t = _Func_8011f54(0, x >> 16, y >> 16);
        e->f10 = t << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;
    } while (i <= 0x1f);
    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    c1 = 0;
    REG_BLDY = c1;
    StartTask(Task_Snow, 0xc8 << 4);
}
