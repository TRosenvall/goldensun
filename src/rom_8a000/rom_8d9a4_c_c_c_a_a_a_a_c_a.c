/* Cluster Task_Transition300..Task_Transition300 extracted from
 * goldensun/asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_c.s.
 *
 * Total .text for this TU = 344 bytes (= 0x158). Never attempted before batch 278.
 * No pins, no flags, no volatile. 138 instructions, 11 candidates, and candidate 1 was already
 * 28 of 146 because the same-stem neighbours carried the whole first half.
 *
 * FROM THE NEIGHBOURS VERBATIM (rom_8d9a4_c_c_c_a_a_a_a_a_b.c and _a_a_c.c): the
 * `signed char *t = *(signed char **)iwram_3001ecc` base, the 0x53a..0x53d field set,
 * `++t[0x53d]` as the ldrb/add/strb/lsl/asr idiom, `divsi3_RAM` reached through a
 * FUNCTION-POINTER LOCAL rather than `/`, and `StopTask` declared `int`. Use
 * `UnknownDMAPrefix()` from include/dma.h for the REG_DMA0SAD prologue -- do not hand-roll it.
 *
 * THE LOOP IS A `goto` LOOP, AND THAT ALONE WAS 27 -> 13. LICM hoisted FIVE invariant constants
 * (the table, 0x3f, 0x508, 1, 0xf) where the ROM keeps only two: `.08.loop` shows all five as
 * insns 488/490/495/497/499 carrying REG_EQUIV, and `.18.greg` hands every one of them a hard
 * register (r8/r9/r10/r12/r14), which is the surplus `push {r7}` / `mov r7, r8`. A `goto` loop
 * emits no loop notes, so nothing is hoisted at all.
 *
 * THE TWO THE ROM *DOES* HOIST THEN HAVE TO COME FROM SOURCE -- `tbl` and `mask` named before
 * the loop: 13 -> 7 -> 4. That is the two-sign LICM rule used in both directions inside one
 * function, which is the clearest instance of it so far: suppress the pass, then write by hand
 * exactly the two hoists the ROM wanted.
 *
 * `i = 0;` PLACED FIRST OF THE THREE INITIALISERS took 4 -> 2. The identical assignment placed
 * third is 4. Position, not naming -- the batch-277 axis again.
 *
 * THE LAST 2: the store `*(u16 *)(t + 0x52a) = t[0x53a] + n;` schedules its address before its
 * value. Splitting the sum into a NEW variable `m` fixes it; REUSING `n` (`n += ...` or
 * `n = t[0x53a] + n`) gets the order right but inverts the operands (`add r0,r3` for `add r3,r0`)
 * and stalls at 3. So a fresh variable and a reused one are different levers here, and the
 * reused-accumulator trick that closed OvlFunc_923_2009ec8 this same batch is the WRONG one for
 * this shape -- read which side of the sum the ROM ties to its output.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

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
extern unsigned char iwram_3001ecc[];
extern unsigned char L9e8ee[] __asm__(".L9e8ee");
extern int StopTask(void *task);
extern int divsi3_RAM();

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueueWindowDma(struct DmaQueue *queue, signed char *t)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (u32)(t + 0xa1 * 8);
        *task++ = 0xc0 << 19;
        *task = 0x84000008;
    }
    SET_IO(REG_IME, savedIme);
}

void Task_Transition300(void);

void Task_Transition300(void)
{
    int (*fp)(int, int);
    signed char *t;
    int n;
    int m;
    int k;
    unsigned int i;
    int val;
    unsigned int b;
    unsigned char *p;
    const unsigned char *tbl;
    int mask;

    t = *(signed char **)iwram_3001ecc;
    if (t[0x53c] != 0) {
        if (t[0x53d] >= t[0x53c]) {
            t[0x53c] = 0;
            StopTask(Task_Transition300);
            UnknownDMAPrefix();
            return;
        }
        n = (t[0x53b] - t[0x53a]) * ++t[0x53d];
        fp = divsi3_RAM;
        n = fp(n, t[0x53c]);
        m = t[0x53a] + n;
        *(unsigned short *)(t + 0x52a) = m;
    }
    k = *(unsigned short *)(t + 0x52a) - 1;
    t[0x539] ^= 1;
    val = 0;
    if (k & 0x20)
        val = 0xf;
    k = (k & 0x1f) * 2;
    i = 0;
    tbl = L9e8ee;
    mask = 0x3f;
loop:
    {
        b = tbl[k & mask];
        p = (unsigned char *)(t + (b >> 1) + 0xa1 * 8);
        if (b & 1)
            *p = (*p & 0xf) | (val << 4);
        else
            *p = (*p & 0xf0) | val;
        k++;
        i++;
        if (i <= 1) goto loop;
    }
    QueueWindowDma(&gDMATaskCount, t);
}
