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

static inline void DMA3_FILL_V(void *dst, u32 _value, unsigned size) {
    u32 value;
    register u32 _v __asm__("r1") = _value;
    register u32 * _src  __asm__("r0") = (&value);
    *_src = _v;
    {
        register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
        register unsigned _dst  __asm__("r1") = (unsigned)(dst);
        register unsigned _cnt  __asm__("r2") = (unsigned)(0x85000000 | (size / 4));
        __asm__ volatile (
            "stmia\t%0!, {%1, %2, %3}\n\t"
            "sub\t%0, #0xc"
            :
            : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
            : "memory"
        );
    }
}

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
    DMA3_FILL_V(buf, 0, 0x410);
    gfx = galloc_ewram(0xe, 0x80 * 8);
    DecompressLZ1(Data_a00b8, gfx);
    slot = AllocSpriteSlot();
    buf[0] = slot;
    buf[1] = UploadSpriteGFX(slot, 0x80 * 4, gfx);
    gfree(0xe);
    i = 0;
    do {
        q = p;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd400;
        s = *(volatile int **)iwram_3001e70;
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
