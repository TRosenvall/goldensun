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
extern unsigned char L68c[] __asm__(".L68c");
extern unsigned char *iwram_3001ebc;

extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern int __StartTask(void *fn, int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void OvlFunc_879_2008238(void);
extern void OvlFunc_879_2008454(void);
extern void OvlFunc_879_20081c0(int a);

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

static inline void SetBldY(struct DmaQueue *queue, u32 t)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)((u32)queue + count * 12 + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0x10 - t;
        *task++ = REG_ADDR_BLDY;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void OvlFunc_879_20082e8(void)
{
    unsigned char *p;
    unsigned char *q;
    int i;

    OvlFunc_879_2008454();
    __CutsceneWait(0x1e);
    *(short *)L68c = 0;
    OvlFunc_879_20081c0(0);
    __StartTask(OvlFunc_879_2008238, 0xc80);
    SetRegAnimDest(&gDMATaskCount, REG_ADDR_DISPCNT, 0x1540);
    SetRegAnimDest(&gDMATaskCount, REG_ADDR_BLDCNT, 0x2fce);
    SetBldY(&gDMATaskCount, 0);
    SetRegAnimDest(&gDMATaskCount, REG_ADDR_BLDALPHA, 0x1010);
    __CutsceneWait(0x78);
    for (i = 0; i <= 0x10; i++) {
        SetBldY(&gDMATaskCount, i);
        __WaitFrames(3);
    }
    p = iwram_3001ebc;
    *(int *)(p + 0x1c0) = 0;
    *(int *)(p + 0x1c8) = 1;
    __MapTransitionIn();
    __WaitMapTransition();
    q = iwram_3001ebc;
    *(int *)(q + 0x1c8) = 0x3c;
}
