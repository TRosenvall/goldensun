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
extern unsigned char iwram_3001eec[];
extern unsigned short iwram_3001ad0[];
extern int gPhysVec[];

extern void _PlaySound(int id);
extern void Func_80008d4(void *dst, s32 len);
extern void StopTask(void *task);
extern void Func_80cd4b4(void);
extern void WaitFrames(unsigned int nframes);
extern int _Func_80c0774(int a, unsigned short b, int c);
extern void _Func_80c0700(unsigned short a, int b);

static inline void SetRegAnimDest(u32 dest, u32 src)
{
    struct DmaQueue *queue;
    u32 savedIme;
    s32 count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *task++ = src;
        queue->count = count + 1;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void AnimEnd(void)
{
    unsigned char *a;
    unsigned char *b;
    void (*fp)(void *, s32);
    unsigned char *d;
    int i;
    int k;
    u32 off;

    a = *(unsigned char **)iwram_3001eec;
    b = *(unsigned char **)(iwram_3001eec - 0x78);
    _PlaySound(0x121);
    off = 0x77a0;
    d = a + off;
    iwram_3001ad0[2] = *(int *)d;
    off = 0x77a4;
    a += off;
    iwram_3001ad0[3] = *(int *)a;
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x78;
    REG_BG2CNT = 0x787;
    fp = Func_80008d4;
    fp((void *)0x6004000, 0x80 << 7);
    StopTask(Func_80cd4b4);
    iwram_3001ad0[3] = 0x20;
    SetRegAnimDest(0x80 << 19, 0x7341);
    REG_BLDCNT = 0;
    WaitFrames(1);
    _Func_80c0774(2, *(unsigned short *)(b + (0xc9 << 3)), 7);
    WaitFrames(1);
    i = 0;
    k = 0;
    do {
        _Func_80c0700(*(unsigned short *)(b + (0xc9 << 3)), 0x15 - k);
        i += 1;
        WaitFrames(1);
        k += 3;
    } while (i != 8);
    SetRegAnimDest(0x80 << 19, 0x7541);
    WaitFrames(1);
}
