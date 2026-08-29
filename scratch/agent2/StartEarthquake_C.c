#include "gba/types.h"
#include "dma.h"

extern void *galloc_ewram(int tag, int size);
extern void gfree(int tag);
extern void DecompressLZ1(const void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *src);
extern int _Func_8011f54(int a, int b, int c);
extern int StartTask(void *fn, int prio);
extern void Task_Earthquake(void);
extern const u8 Data_a00b8[];
extern unsigned int iwram_3001e70;

void StartEarthquake(void)
{
    int *buf;
    void *gfx;
    int *p;
    int *q;
    volatile int *s;
    int slot;
    unsigned int i;
    int r;
    int a;
    int b;

    buf = (int *)galloc_ewram(0x1d, 0x82 * 8);
    p = buf + 2;
    DMA3_FILL(buf, 0, 0x410);
    gfx = galloc_ewram(0xe, 0x80 * 8);
    DecompressLZ1(Data_a00b8, gfx);
    slot = AllocSpriteSlot();
    buf[0] = slot;
    buf[1] = UploadSpriteGFX(slot, 0x80 * 4, gfx);
    gfree(0xe);
    i = 0;
    do {
        s = *(volatile int **)iwram_3001e70;
        q = p;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd400;
        a = s[0];
        b = s[2];
        a >>= 16;
        p[3] = 0;
        p[5] = 0;
        b >>= 16;
        r = _Func_8011f54(0, a, b);
        p[4] = r << 16;
        *(u16 *)((char *)p + 0x1c) = (i & 0xf) + 1;
        p += 8;
        i++;
    } while (i <= 0x1f);
    StartTask(Task_Earthquake, 0xc8 * 16);
}
