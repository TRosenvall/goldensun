#include "gba/types.h"
#include "dma.h"

extern void *galloc_ewram(int tag, int size);
extern void gfree(int tag);
extern void DecompressLZ1(const void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *src);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void *fn, int prio);
extern void Task_Earthquake(void);
extern const u8 Data_a00b8[];
extern unsigned int iwram_3001e70;

void StartEarthquake(void)
{
    int *buf;
    void *gfx;
    int *p;
    int *q;
    int *s;
    int slot;
    unsigned int i;
    int r;

    buf = (int *)galloc_ewram(0x1d, 0x82 * 8);
    DMA3_CLEAR(buf, 0x410);
    gfx = galloc_ewram(0xe, 0x80 * 8);
    DecompressLZ1(Data_a00b8, gfx);
    slot = AllocSpriteSlot();
    buf[0] = slot;
    buf[1] = UploadSpriteGFX(slot, 0x80 * 4, gfx);
    gfree(0xe);
    p = buf + 2;
    i = 0;
    do {
        q = p;
        *q++ = 0;
        s = *(int **)iwram_3001e70;
        *q++ = 0x40000400;
        *q = 0xd400;
        p[3] = 0;
        p[5] = 0;
        r = _Func_8011f54(0, s[0] >> 16, s[2] >> 16);
        p[4] = r << 16;
        *(u16 *)((char *)p + 0x1c) = (i & 0xf) + 1;
        p += 8;
        i++;
    } while (i <= 0x1f);
    StartTask(Task_Earthquake, 0xc8 * 16);
}
