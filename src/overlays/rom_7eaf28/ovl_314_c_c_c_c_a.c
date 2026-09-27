/* OvlFunc_960_2008f50 and OvlFunc_960_2009094  --  0x02008f50 / 0x02009094, the
 * code of asm/overlays/rom_7eaf28/ovl_314_c_c_c_c.s; its .data tail stays in
 * ovl_314_c_c_c_c_c.s, and the split's two code pieces are merged back into this
 * one object. 2009094 was a fresh target; 2008f50 comes OUT OF A PARK. Verified
 * from this combined file with objcmp --whole.
 *
 * LOCK_IME and the SetRegAnimDest/SetBldAlpha inlines are copied from
 * src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c; the byte stores at +0x25/+0x26
 * are struct members, or move2add chains the second address off the first.
 */
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

extern struct DmaQueue gDMATaskCount;

struct Sprite {
    u8 pad0[5];
    u8 f5;
    u8 pad6[0x1f];
    u8 f25;
    u8 f26;
    u8 pad27;
    struct Sprite *f28;
};


extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8091ff0(int id);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void SetRegAnimDest(struct DmaQueue *queue, u32 dest, u32 src)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = src;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void SetBldAlpha(struct DmaQueue *queue, int t)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = ((0x10 - t) << 8) | t;
        *task++ = REG_ADDR_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void OvlFunc_960_2008f50(void)
{
    int eva;
    int i;
    unsigned char *a;

    eva = 0;
    if (__GetFlag(0x301))
        __SetFlag(0x206);
    if (__GetFlag(0x302))
        __SetFlag(0x207);
    if (__GetFlag(0x303))
        __SetFlag(0x208);
    if (__GetFlag(0x304))
        __SetFlag(0x209);
    if (__GetFlag(0x305))
        __SetFlag(0x20a);
    for (i = 8; i <= 0xc; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0) {
            if (__GetFlag(0x109) == 0) {
                *(int *)(a + 0x18) = 0x800;
                *(int *)(a + 0x1c) = 0x800;
            }
            (*(struct Sprite **)(a + 0x50))->f26 = 0;
        }
    }
    SetRegAnimDest(&gDMATaskCount, REG_ADDR_BLDCNT, 0x3f42);
    if (__GetFlag(0xd0 << 2)) {
        eva = 0x10;
        __Func_8091ff0(0xf4);
    }
    SetBldAlpha(&gDMATaskCount, eva);
}

void OvlFunc_960_2009094(void)
{
    int eva;
    int i;
    unsigned char *a;
    struct Sprite *s;
    struct Sprite *t;

    eva = 0;
    if (__GetFlag(0x311))
        __SetFlag(0x206);
    if (__GetFlag(0x312))
        __SetFlag(0x207);
    if (__GetFlag(0x313))
        __SetFlag(0x208);
    for (i = 8; i <= 10; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0) {
            if (__GetFlag(0x109) == 0) {
                *(int *)(a + 0x18) = 0x800;
                *(int *)(a + 0x1c) = 0x800;
            }
            s = *(struct Sprite **)(a + 0x50);
            s->f26 = 0;
        }
    }
    a = __MapActor_GetActor(0xb);
    if (a != 0) {
        s = *(struct Sprite **)(a + 0x50);
        t = s->f28;
        if (t != 0)
            t->f5 = 0xa;
        s->f25 = 1;
        s->f26 = 0;
    }
    if (__GetFlag(0x315))
        __SetFlag(0x9b7);
    SetRegAnimDest(&gDMATaskCount, REG_ADDR_BLDCNT, 0x3f42);
    if (__GetFlag(0xd0 << 2)) {
        eva = 0x10;
        __Func_8091ff0(0xf4);
    }
    SetBldAlpha(&gDMATaskCount, eva);
}
