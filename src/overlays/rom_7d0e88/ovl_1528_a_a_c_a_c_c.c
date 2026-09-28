#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct Tile {
    unsigned char f0;
    unsigned char f1_lo : 6;
    unsigned char f1_hi : 2;
    unsigned char f2;
    unsigned char f3;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char f23;
    unsigned char pad24[0x28 - 0x24];
    int f28;
    unsigned char pad2c[0x55 - 0x2c];
    unsigned char f55;
    unsigned char pad56[0x70 - 0x56];
};

struct E {
    int f0;
    int f4;
    int f8;
    struct Tile fc;
    int f10;
};

extern struct E bss_36d0[];
extern int L3720[3] __asm__(".L3720");

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern int __TestCollision(struct Actor *a, int *v);
extern int __Func_8012038(int a, int b, int c);
extern void OvlFunc_947_2008d78(struct Actor *a);
extern void OvlFunc_947_2008da8(struct Actor *a, int b);
extern void OvlFunc_947_2008fcc(int a, int b, int c, struct Tile *dst);
extern void OvlFunc_947_200901c(int a, int b, int c, struct Tile *dst);
extern void OvlFunc_947_2009074(int slot, int b);
extern void OvlFunc_947_2009174(int slot);

void OvlFunc_947_2009be8(void)
{
    struct Actor tmp;
    int tmpE[3];
    vu32 *dma;
    unsigned int i;
    unsigned int j;
    struct Actor *a;
    struct Actor *b;

    for (i = 0; i <= 2; i++) {
        a = __MapActor_GetActor(i + 8);
        for (j = i; j <= 3; j++) {
            b = __MapActor_GetActor(j + 8);
            if (a->fc > b->fc || a->f10 >= b->f10) {
                DMA3_COPY(b, &tmp, 0x70);
                dma = (vu32 *)&REG_DMA3SAD;
                while (dma[2] & 0x80000000)
                    ;
                DMA3_COPY(a, b, 0x70);
                dma = (vu32 *)&REG_DMA3SAD;
                while (dma[2] & 0x80000000)
                    ;
                DMA3_COPY(&tmp, a, 0x70);
                dma = (vu32 *)&REG_DMA3SAD;
                while (dma[2] & 0x80000000)
                    ;
                DMA3_COPY(&bss_36d0[j], tmpE, 0x10);
                dma = (vu32 *)&REG_DMA3SAD;
                while (dma[2] & 0x80000000)
                    ;
                DMA3_COPY(&bss_36d0[i], &bss_36d0[j], 0x10);
                dma = (vu32 *)&REG_DMA3SAD;
                while (dma[2] & 0x80000000)
                    ;
                DMA3_COPY(tmpE, &bss_36d0[i], 0x10);
                dma = (vu32 *)&REG_DMA3SAD;
                while (dma[2] & 0x80000000)
                    ;
                if (__GetFlag(bss_36d0[i].f10) && !__GetFlag(bss_36d0[j].f10)) {
                    __ClearFlag(bss_36d0[i].f10);
                    __SetFlag(bss_36d0[j].f10);
                } else if (!__GetFlag(bss_36d0[i].f10) && __GetFlag(bss_36d0[j].f10)) {
                    __SetFlag(bss_36d0[i].f10);
                    __ClearFlag(bss_36d0[j].f10);
                }
            }
        }
    }
}

void OvlFunc_947_2009d84(void)
{
    struct Tile loc;
    struct Actor *a;
    struct Actor *o;
    unsigned int n;
    unsigned int m;
    int k;

    for (n = 8; n <= 0xb; n++) {
        a = __MapActor_GetActor(n);
        a->f22 = 2;
        k = n - 8;
        if ((a->f8 >> 20) == bss_36d0[k].f0 && (a->f10 >> 20) == bss_36d0[k].f8
            && a->f28 == 0)
            continue;
        DMA3_COPY(&a->f8, L3720, 0xc);
        {
            vu32 *dma = (vu32 *)&REG_DMA3SAD;
            while (dma[2] & 0x80000000)
                ;
        }
        if (__TestCollision(a, L3720) == -1)
            a->f55 = 3;
        OvlFunc_947_200901c(0, bss_36d0[k].f0, bss_36d0[k].f8, &bss_36d0[k].fc);
        OvlFunc_947_200901c(2, bss_36d0[k].f0, bss_36d0[k].f8, &bss_36d0[k].fc);
        if (a->f55 & 1) {
            if (__Func_8012038(2, a->f8, a->f10) == 0x32) {
                __PlaySound(0xbd);
                a->f23 &= 0xfe;
                OvlFunc_947_2009074(n, 1);
                a->f23 |= 1;
            } else if (__Func_8012038(2, a->f8, a->f10) == 0x33) {
                OvlFunc_947_2008da8(a, 0);
                __PlaySound(0xbd);
                a->fc = 0;
                a->f23 &= 0xfe;
                OvlFunc_947_2009174(n);
                a->f8 = 0;
                a->fc = 0;
                a->f10 = 0;
                a->f23 |= 1;
            } else {
                OvlFunc_947_2008d78(a);
            }
            a->f55 = 0;
        }
        OvlFunc_947_2008fcc(0, a->f8 >> 20, a->f10 >> 20, &bss_36d0[k].fc);
        if (a->fc >= 0) {
            OvlFunc_947_2008fcc(0, 0x1b, (a->fc >> 20) + 6, &loc);
            OvlFunc_947_200901c(0, a->f8 >> 20, a->f10 >> 20, &loc);
            loc.f1_hi = bss_36d0[k].fc.f1_hi;
            OvlFunc_947_200901c(2, a->f8 >> 20, a->f10 >> 20, &loc);
        }
        bss_36d0[k].f0 = a->f8 >> 20;
        bss_36d0[k].f4 = a->fc >> 20;
        bss_36d0[k].f8 = a->f10 >> 20;
        for (m = 0; m <= 3; m++) {
            if (m == k)
                continue;
            __ClearFlag(bss_36d0[m].f10);
            o = __MapActor_GetActor(m + 8);
            if ((a->f8 >> 20) == (o->f8 >> 20) && (a->f10 >> 20) == (o->f10 >> 20)
                && a->fc > o->fc)
                __SetFlag(bss_36d0[m].f10);
        }
    }
    OvlFunc_947_2009be8();
}
