/* StartSnow  --  0x08094da0, split out of asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a.s
 * (fifth of seven: Task_Rain, Task_Thunder, StartRain, Task_Snow stay in _a.s,
 * Task_Earthquake and StartEarthquake in _c.s). Matched from scratch.
 *
 * FAKEMATCH -- two pins, booked in fakematch.txt: a file-local inline DMA3_CLEAR
 * with the zero pinned to r1, and `register int *q __asm__("r1")`. Dropping
 * either costs 2 or 12. The r1 zero pin also closes residue 3 of the
 * StartEarthquake park (src/non_matching/rom_8a000/809509c.c); StartRain and
 * StartEarthquake are worth reopening with both.
 *
 * `do { } while (0)` around the three BLD halfword stores stops sched2 hoisting
 * StartTask's `mov r1,#0xc8` above them. StartTask is a local `void` extern, not
 * task.h's s32 one -- do not include task.h. `(i & 0xf) + 1` stored straight
 * into a u16 reproduces the pooled `ldr =0xf`.
 */
#include "dma.h"

extern void *galloc_ewram(int kind, int size);
extern void gfree(int kind);
extern void DecompressLZ1(const void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern int _Func_8011f54(int a, int b, int c);
extern int **iwram_3001e70;
extern unsigned char Data_a001e[];
extern void Task_Snow(void);
extern void StartTask(void (*f)(void), int pri);

static inline void Dma3Clear(void *dst, unsigned size)
{
    u32 value;
    register u32 zero __asm__("r1") = 0;
    register u32 *_src __asm__("r0") = &value;
    *_src = zero;
    {
        register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
        register unsigned _dst __asm__("r1") = (unsigned)(dst);
        register unsigned _cnt __asm__("r2") = (unsigned)(0x85000000 | (size / 4));
        __asm__ volatile (
            "stmia\t%0!, {%1, %2, %3}\n\t"
            "sub\t%0, #0xc"
            :
            : "l" (_base), "l" (_src), "l" (_dst), "l" (_cnt)
            : "memory"
        );
    }
}

void StartSnow(void)
{
    int *s;
    int *e;
    register int *q __asm__("r1");
    int *w;
    void *buf;
    unsigned int i;
    int x;
    int z;
    vu16 *r;
    int v;

    s = galloc_ewram(0x1d, 0x410);
    e = s + 2;
    Dma3Clear(s, 0x410);
    buf = galloc_ewram(0xe, 0x400);
    DecompressLZ1(Data_a001e, buf);
    s[0] = AllocSpriteSlot();
    s[1] = UploadSpriteGFX(s[0], 0x300, buf);
    gfree(0xe);
    for (i = 0; i < 0x20; i++) {
        w = *iwram_3001e70;
        q = e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd400;
        x = w[0];
        z = w[2];
        e[3] = x;
        e[5] = z;
        e[4] = _Func_8011f54(0, x >> 16, z >> 16) << 16;
        *(unsigned short *)(e + 7) = (i & 0xf) + 1;
        e += 8;
    }
    do {
        r = &REG_BLDCNT;
        v = 0x3f00;
        *r = v;
        v = 0x1008;
        r++;
        *r = v;
        r++;
        *r = 0;
    } while (0);
    StartTask(Task_Snow, 0xc80);
}
