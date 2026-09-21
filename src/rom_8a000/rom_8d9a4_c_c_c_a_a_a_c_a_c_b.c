/* Cluster Func_80908e0..Func_80908e0 extracted from
 * goldensun/asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c.s.
 *
 * Total .text for this TU = 380 bytes (= 0x17c). Never attempted before batch 278.
 * No pins, no flags. 164 instructions, 9 candidates, and candidate 1 was 12 of 173 on family
 * transfer alone.
 *
 * FROM THE FAMILY: the iwram_3001ed0 base, the `++b[0x2a02] < b[0x2a01]` signed-char idiom, the
 * Func_8001af8 call through a pointer local (`bl _call_via_r3`), and the gDMATaskCount inline --
 * but the inline needs TWO SPECIALISATIONS, because push 1's source is computed BEFORE LOCK_IME
 * (so it is an inline argument) while push 2's is computed INSIDE the guard (so it must be built
 * in the inline body). Worth knowing before copying one of them twice.
 *
 * `ldrh` plus `lsl #16 / asr #21` is the UN-FOLDED sign extension: the buffer is
 * `unsigned short *` and the shift operand is `(short)p[1]`. That is the batch-278 rule that
 * ldrsh-vs-ldrh is decided by the VARIABLE's width, with the cast written at the point of use.
 *
 * THE LAST 12 WERE A PAIR OF LEVERS THAT ONLY WORK TOGETHER, which is the
 * pair-before-discarding rule paying again:
 *   naming the mask (`m = 0x7c00;`) IMMEDIATELY BEFORE the pointer assignment   12 -> 10
 *   `i = 0xe0 << 1;` moved BETWEEN `m` and `p`                                  10 -> 0
 *   the same `i` move WITHOUT the named mask                                    14
 * Naming the mask EARLIER (before `d`) is 22. So both the naming and its position matter, and
 * measuring either alone would have rejected it.
 *
 * The mechanism: the constant pseudo for `b + 0x380` is short-lived and wins local_alloc's
 * priority, while LICM APPENDS the mask to the preheader -- so the mask has to be named into the
 * ROM's slot by hand.
 *
 * Measured inert or worse: a distinct pointer for the second loop (12); a distinct counter (19);
 * `p` declared before `d` (25).
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
extern unsigned char iwram_3001ed0[];
extern int _GetFlag(int id);
extern void Func_8001af8();

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueuePaletteDma(struct DmaQueue *queue, const void *src, void *dest)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (u32)src;
        *task++ = (u32)dest;
        *task = 0x84000070;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePaletteDma2(struct DmaQueue *queue, signed char *s)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (u32)(s + 0x24c0);
        *task++ = (void *)0x5000200;
        *task = 0x84000070;
    }
    SET_IO(REG_IME, savedIme);
}

void Func_80908e0(void);

void Func_80908e0(void)
{
    void (*fp)(void *, void *, int);
    signed char *b;
    unsigned short *q;
    unsigned short *p;
    unsigned short *d;
    signed char *s;
    int i;
    int m;

    b = *(signed char **)iwram_3001ed0;
    q = (unsigned short *)(b + (0xc4 << 5));
    if (_GetFlag(0xa9 << 1) != 0)
        return;
    if (b[0x2a01] == 0)
        return;
    if (++b[0x2a02] < b[0x2a01]) {
        p = (unsigned short *)(b + (0xe0 << 2));
        for (i = 0; i <= 0x53f; i++) {
            *p += *q;
            q++;
            p++;
        }
    } else {
        fp = Func_8001af8;
        fp(b + (0xe0 << 2), b + (0xe0 << 4), 0xa8 << 4);
        b[0x2a01] = 0;
    }
    d = (unsigned short *)(b + (((unsigned char *)b)[0x2a00] ^ 1) * 0x380 + (0x8c << 6));
    m = 0x7c00;
    i = 0xe0 << 1;
    p = (unsigned short *)(b + (0xe0 << 2));
    do {
        *d = (p[0] & m) | (((short)p[1] >> 5) & 0x3e0) | (((short)p[2] >> 10) & 0x1f);
        p += 3;
        d++;
        i--;
    } while (i != 0);
    ((unsigned char *)b)[0x2a00] ^= 1;
    s = b + ((unsigned char *)b)[0x2a00] * 0x380;
    QueuePaletteDma(&gDMATaskCount, s + (0x8c << 6), (void *)(0xa0 << 19));
    QueuePaletteDma2(&gDMATaskCount, s);
}
