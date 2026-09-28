#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern int L3a68[] __asm__(".L3a68");
extern int L3a90[] __asm__(".L3a90");

struct Inner {
    unsigned char pad0[0x26];
    unsigned char f26;
};

struct Actor {
    unsigned char pad0[0x50];
    struct Inner *f50;
    unsigned char pad54;
    unsigned char f55;
};

struct Ent {
    struct Actor *f0;
    unsigned char pad4[0x18];
    int f1c;
    int f20;
    unsigned char f24;
    unsigned char pad25[3];
};

extern unsigned char *__galloc_ewram(int tag, int size);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_800c548(struct Actor *a, int n);
extern int __StartTask(void *fn, int pri);
extern void OvlFunc_897_200b01c(void);

void OvlFunc_897_200b30c(int slot, unsigned int n)
{
    unsigned char *p;
    struct Ent *e;
    struct Actor *a;
    unsigned int i;
    int z;

    p = __galloc_ewram(0x21, 0xca << 1);
    e = (struct Ent *)p;
    DMA3_CLEAR(p, 0xca << 1);
    if (n > 0xa)
        n = 0xa;
    i = 0;
    if (n != 0) {
        z = 0;
        do {
            a = __MapActor_GetActor(slot);
            a->f50->f26 = z;
            e->f0 = a;
            a->f55 = z;
            __Func_800c548(__MapActor_GetActor(slot), 1);
            e->f1c = L3a68[i];
            e->f20 = -L3a90[i];
            e->f24 = 3;
            i++;
            e++;
            slot++;
        } while (i != n);
    }
    *(unsigned short *)(p + (0xc8 << 1)) = n;
    __StartTask(OvlFunc_897_200b01c, 0xc8 << 4);
}
