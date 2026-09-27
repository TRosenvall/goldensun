/* Func_809bb64  --  0x0809bb64, was asm/rom_8a000/rom_9bb64_a.s (this function
 * alone), so it converts whole. Matched from scratch; the header stores must be
 * written in index order (the best other of 24 orders reached 2).
 */
#include "dma.h"

struct WM {
    unsigned short slot;
    unsigned short f2;
    int f4;
    int f8;
    unsigned char pad0c[6];
    unsigned short f12;
    unsigned char pad14[4];
    int f18;
    int f1c;
};

extern unsigned char gBuffer[];
extern unsigned char gState[];
struct SpriteSlot { unsigned short a; unsigned short b; };
extern struct SpriteSlot gSpriteSlots[];
extern int *Func_8004970(int size);
extern unsigned char *MapActor_GetActor(int slot);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern void free(void *p);
extern int _GetFlag(int id);
extern int _Func_80209b0(void);
extern int _CreateUIBox(int a, int b, int c, int d, int e);

void Func_809bb64(void)
{
    int *h;
    struct WM *w;
    unsigned int *p;
    unsigned char *a;
    unsigned char *g;
    int attr;
    unsigned int i;
    int t;
    unsigned int s;

    h = Func_8004970(0x20);
    w = (struct WM *)gBuffer;
    p = (unsigned int *)(gBuffer + 0x20);
    g = gState;
    g += 0xfa << 1;
    a = MapActor_GetActor(*(int *)g);
    w->slot = AllocSpriteSlot();
    DMA3_CLEAR(h, 0x80);
    h[0] = 0xff;
    h[1] = 0x1ff;
    h[2] = 0x110;
    h[8] = 0x44;
    h[9] = 0x144;
    h[10] = 0x110;
    h[0x10] = 0x77;
    h[0x11] = 0x177;
    h[0x12] = 0x110;
    h[0x18] = 0xff0;
    h[0x19] = 0xffff;
    h[0x1a] = 0x1ffff;
    h[0x1b] = 0x11ff0;
    h[0x1c] = 0x1100;
    attr = UploadSpriteGFX(w->slot, 0x80, h) | 0x400;
    for (i = 0; i <= 0x41; i++) {
        *p++ = 0;
        *p++ = 0;
        *p++ = attr;
    }
    free(h);
    if (_GetFlag(0x11c)) {
        w->f4 = 0xf0 << 15;
        w->f8 = 0xa0 << 15;
    } else {
        w->f4 = ((((*(int *)(a + 8) + (int)0xf0000000) >> 16) * 240) / 0x1000) << 16;
        w->f8 = ((*(short *)(a + 0x12) * 160) / 0x1000) << 16;
    }
    w->f2 = _Func_80209b0();
    s = gSpriteSlots[w->f2].b;
    t = _CreateUIBox(0, 0, 0, 0, 2);
    w->f12 = 0xffff;
    w->f18 = 0x10000;
    w->f1c = t;
    *p++ = 0;
    *p++ = 0x40000000;
    {
        unsigned int k = 0x400;
        k |= s >> 5;
        *p = k;
    }
}
