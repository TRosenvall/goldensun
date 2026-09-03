/* Debug_TransferTest  --  0x0800679c
 *
 * The whole of goldensun/asm/rom_c0/rom_5cf8_c.s: one function, no data.
 *
 * A developer transfer harness. Fill a strip of palette RAM with a descending
 * pattern, clear a buffer, then loop forever: A and B fire two different
 * transfers, the third button spins a busy loop 10000 times, and when a flag
 * clears the buffer is DMAed to VRAM and the outer pass restarts. It never
 * returns -- the prologue pushes lr and there is no epilogue.
 *
 * FOUR LEVERS, and two of them are levers found earlier in this same batch,
 * which is why this one went from 24 differing to exact in five screens.
 *
 * THE KEY GLOBAL IS volatile. The ROM reads it three times through a held base
 * register rather than once into a variable, which is the read-count rule's
 * usual picture, but plain reads got the loaded value and the mask constant in
 * the wrong registers and would not move under any spelling. `volatile` fixes
 * both at once -- 24 aligned to 6. Worth trying early on any input-polling
 * loop, and the corpus already spells the sibling key global that way.
 *
 * THE OUTER LOOP IS A goto, AND THAT IS LOAD-BEARING. Written as a nested
 * `for (;;)`, gcc hoists the key global's address out of BOTH loops; the ROM
 * reloads it once per outer pass, inside. A backward `goto` is not a natural
 * loop to loop.c, so it gets no invariant motion across it and the address
 * stays in the inner loop's preheader, which is the ROM. 4 aligned to 2.
 *
 * This is the exception to the note recorded earlier this batch that
 * `for (init; ; inc)` should be tried before a `goto`. That note is about
 * getting an UN-ROTATED loop shape, and it stands. This is the other reason to
 * reach for `goto`: to DENY loop-invariant motion. Two different jobs, and the
 * tell for this one is a pool load the ROM repeats per iteration.
 *
 * THE SCRATCH WORD IS A char ARRAY. The ROM does `mov r0, sp / mov r3, #0 /
 * str r3, [r0]` -- it materialises the frame address into a register and stores
 * through it, where an `int` local gives the folded `str r3, [sp]`. A four-byte
 * `char` array with a cast store keeps the address in a register, which is the
 * recorded byte-granular address-taken tell. A pointer local over the array
 * works identically; the array alone is the least source for the same output.
 *
 * The register rotation in the fill loop is ordinary birth order: the ROM ranks
 * the stored value above the pointer above the counter, so the counter is
 * assigned FIRST (longest range, lowest priority) and the value LAST. Four
 * orders measured, 24 / 20 / 19 / 14.
 *
 * The DMA is include/dma.h's DMA3_COPY; its count word matches with size 0x280.
 */
#include "dma.h"

extern unsigned char gBuffer[];
extern volatile int gKeyHeld;
extern unsigned int ewram_20023ac;

extern void _PlaySound(int id);
extern void Func_8005d10(void);
extern void CpuSet(void *src, void *dst, int cnt);
extern void Func_8006384(int n);
extern void Func_8006408(unsigned char *p);
extern void Func_80063bc(int a, int b);
extern void Func_8006798(void);
extern void WaitFrames(int n);

void Debug_TransferTest(void)
{
    short *p;
    int v;
    int i;
    char buf[4];
    int j;
    int *q;

    _PlaySound(3);
    Func_8005d10();
    i = 0x13;
    p = (short *)0x6002426;
    v = 0xfffff093;
    do {
        i--;
        *p = v;
        p--;
        v--;
    } while (i >= 0);
    *(int *)buf = 0;
    CpuSet(buf, gBuffer, 0x5000100);
    Func_8006384(3);
restart:
    Func_8006408(gBuffer);
    for (;;) {
        if (gKeyHeld & 1)
            Func_80063bc(0x80 << 20, 0xa0 << 2);
        if (gKeyHeld & 2)
            Func_80063bc(0x8001000, 0xa0 << 2);
        if (gKeyHeld & 8) {
            j = 0x270f;
            do {
                j--;
                Func_8006798();
            } while (j >= 0);
        }
        if (ewram_20023ac == 0) {
            DMA3_COPY(gBuffer, (void *)0x6001000, 0x280);
            goto restart;
        }
        WaitFrames(1);
    }
}
