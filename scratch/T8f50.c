#include "gba/types.h"
#include "gba/io.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

struct Sprite {
    unsigned char pad0[0x26];
    unsigned char f26;
};

struct Actor {
    unsigned char pad0[0x18];
    int f18;
    int f1c;
    unsigned char pad1[0x30];
    struct Sprite *sprite;
};

extern struct DmaQueue gDMATaskCount;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8091ff0(int n);

void OvlFunc_960_2008f50(void)
{
    struct Actor *a;
    struct Sprite *s;
    struct DmaQueue *q;
    u32 savedIme;
    s32 count;
    u32 *task;
    int i;
    int v;
    int w;

    v = 0;
    if (__GetFlag(0x301))
        __SetFlag(0x206);
    if (__GetFlag(0x302))
        __SetFlag(0x207);
    if (__GetFlag(0x303))
        __SetFlag(0x82 << 2);
    if (__GetFlag(0xc1 << 2))
        __SetFlag(0x209);
    if (__GetFlag(0x305))
        __SetFlag(0x20a);
    w = 0x80 << 4;
    for (i = 8; i <= 0xc; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0) {
            if (__GetFlag(0x109) == 0) {
                a->f18 = w;
                a->f1c = w;
            }
            s = a->sprite;
            s->f26 = 0;
        }
    }
    q = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = q->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)q + 4);
        *task++ = 0x3f42;
        q->count = count + 1;
        *task++ = (u32)&REG_BLDCNT;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
    if (__GetFlag(0xd0 << 2)) {
        v = 0x10;
        __Func_8091ff0(0xf4);
    }
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = q->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)q + 4);
        q->count = count + 1;
        *task++ = ((0x10 - v) << 8) | v;
        *task++ = (u32)&REG_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}
