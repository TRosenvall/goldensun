/* OvlFunc_880_2008154 -- 0x02008154
 *
 * A screen-fade task: each frame, step a counter, queue a DMA write of the
 * blend control word and another of the blend alpha built from the counter,
 * and retire itself once the counter passes 0xf.
 *
 * THE QUEUE PUSH is src/non_matching/rom_c9000/AnimEnd.c's inline, copied
 * verbatim -- the DmaTransfer/DmaQueue layout, the `count * 12 + queue + 4`
 * task address, the count store BEFORE the source store, and
 * `SET_IO(REG_IME, REG_ADDR_IME)` for the `strh r0, [r0]` that disables
 * interrupts by storing the port's own address into it.
 *
 * DO NOT PASS A COMPUTED VALUE AS AN INLINE'S ARGUMENT. gcc evaluates the
 * arguments BEFORE the inlined body, so building the alpha word at the call
 * hoisted the whole expression above the interrupt guard. Specialising a second
 * inline that builds the value at its use, inside the guard, put it where the
 * ROM has it: 50 differing to 38. This is a general trap with inlines -- an
 * argument is not "at the call site", it is before everything the inline does.
 *
 * WIDEN THE VARIABLE SO THE CAST SURVIVES. Held as `u16`, gcc folds the
 * narrowing away at tree level and compares the register directly. The ROM's
 * `lsl r0, r5, #0x10 / lsr r1, r0, #0x10` is the UN-folded zero extension, so
 * the counter has to be an `int` with explicit `(u16)` casts at both uses.
 * 38 differing to 5.
 *
 * `do { } while (0)` IS A SCHEDULING BARRIER THAT COSTS NOTHING. The last five
 * lines were purely the order of two independent pool loads: the ROM
 * materialises the queue address before the IME port address, gcc did the
 * reverse. Wrapping the save-and-disable pair in `do/while(0)` splits the block
 * into two scheduling regions and closes it exactly -- with no instruction
 * emitted, unlike the volatile-asm barrier, which was also tried and lands the
 * pool load either too early (9 differing) or still needs help (3).
 *
 * A PIN IS BADLY INERT HERE: pinning the queue pointer to r4 rewrote the whole
 * allocation, 68 of 73. Worth recording beside the cases where a pin is the
 * answer -- it is not a free thing to try on a function with an inlined body.
 *
 * Verified with tools/objcmp.py: 168 bytes, 74 encodings and 4 relocations
 * identical. This overlay's linker script aliases only __divsi3 and __modsi3,
 * not __umodsi3, and the function has no division, so no phantom arises.
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
extern unsigned short L16b0 __asm__(".L16b0");
extern void __StopTask(void *task);

/* The queue push runs with interrupts off. Wrapping the save-and-disable in
 * do/while(0) is not cosmetic: it is what puts the pool load of the queue
 * address ahead of the load of REG_ADDR_IME. */
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
        *task++ = ((0x10 - (u16)t) << 8) | (u16)t;
        *task++ = REG_ADDR_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void OvlFunc_880_2008154(void)
{
    int x;

    x = ++L16b0 >> 1;
    SetRegAnimDest(&gDMATaskCount, REG_ADDR_BLDCNT, 0x2e51);
    SetBldAlpha(&gDMATaskCount, x);
    if ((u16)x > 0xf)
        __StopTask(OvlFunc_880_2008154);
}
