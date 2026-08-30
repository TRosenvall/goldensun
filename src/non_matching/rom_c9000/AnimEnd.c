/* AnimEnd  [rom_c9000]  --  asm/rom_c9000/rom_cd508_a_c.s
 *
 * BLOCKER: a queue-push inline, four residues. 48 of 126. The prologue
 * through the first queue push matches instruction for instruction.
 *
 * THIS FUNCTION SETTLED THE POOL QUESTION FOR THE WHOLE 516-FUNCTION CLASS.
 * Its ROM has TWO branch-over-pool sites. At 64 differing both were ABSENT
 * from our output; once the instruction count converged, BOTH APPEARED
 * SPONTANEOUSLY AND MATCHED. The pool branch is a consequence of code length,
 * not of a source construct. Screen the class normally.
 *
 * A LEVER WORTH REUSING: let loop-invariant motion make the ROM's duplicate.
 * The ROM computes one expression twice. Hoisting it by hand before the loop
 * makes gcc fold both into one; leaving the full expression INSIDE the loop
 * body makes LICM hoist it into the preheader as a second, independent
 * computation -- which is exactly what the ROM has. 64 -> 49.
 *
 * WHAT IS LEFT:
 *   - 9 lines at two sites: a pool address is hoisted to the top of the block
 *     in the ROM; gcc propagates it into its use and the scheduler lands it
 *     four slots later. The `__asm__ volatile` barrier from the existing
 *     fakematch in src/rom_c0/rom_3650_c_b.c moves it within two lines and no
 *     closer; pushing the barrier out to the call site regressed to 100.
 *   - 4 lines at two sites: an `add` and a `strh` swapped. Six inline
 *     spellings tried; only one moved the count, by 1.
 *   - 2 lines: two instructions swapped.
 *   - 1 line: a redundant label. Ours emits the pool-branch target and the
 *     loop head as two labels where the ROM has one. It costs no bytes but
 *     tryc counts it, which is why our 127 reads as one MORE line than the
 *     ROM's 126.
 *
 * AnimStart and AnimStart2 in this same .s are structurally right and far out
 * (244 of 284, 237 of 274). They are this function's queue inline four times
 * over plus two residues of their own, so they will not close until this one
 * does. Do AnimEnd first; the other two are mechanical from it. Also
 * confirmed by probe: gcc-2.96 thumb DOES allocate r12 and lr, so their tile
 * loop is ordinary allocation and not hand-written asm.
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
        *(u16 *)queue = count + 1;
        *task++ = src;
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
