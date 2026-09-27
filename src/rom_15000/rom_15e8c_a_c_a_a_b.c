/* Func_8015fb8 + Func_8016018  --  0x08015fb8..0x080160fc
 *
 * ONE translation unit. It replaces this file's earlier transcription of
 * Func_8015fb8 alone (register binding to r9 plus a volatile slot) and takes
 * Func_8016018 out of asm/rom_15000/rom_15e8c_a_c_a_a_c_a.s, split for it;
 * Func_80160fc stays in rom_15e8c_a_c_a_a_c_a_c.s.
 *
 * Func_8015fb8 IS A NESTED FUNCTION OF Func_8016018, as the header of
 * rom_15e8c_a_c_a_a_b.c predicted. Written that way, gcc-2.96 emits the nested
 * function first (it is compiled when its body closes, before the parent), so
 * the ROM order 0x15fb8 then 0x16018 falls out; its r9 save / `mov r2, r9` /
 * stack slot is the static chain, and the parent's
 * `add rN, sp, #4 / mov r9, rN` before each call is gcc passing its frame.
 * The nested body is byte-identical to the old transcription WITHOUT the
 * `register ... __asm__("r9")` binding or the volatile slot.
 *
 * The nested function's symbol becomes the LOCAL `Func_8015fb8.0`; the only
 * caller in the tree is Func_8016018, so nothing else needs the global name.
 *
 * The one lever in the parent: the fill value of the trailing 3-byte loop
 * must be a block-local `int k = 4`. With a literal 4, `mov r3, #2` is
 * scheduled before `mov r2, #4` (2 differing); every loop-shape rewrite tried
 * (do/while, pointer walk, up-count) was worse or equal.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern int StartTask(void *fn, int pri);
extern void Func_8015ef4(void);
extern void Func_8017464(int mode);
extern void Func_80160fc(void);
extern void Func_80008d4(void *dst, int size);

void Func_8016018(int mode)
{
    unsigned char *p;
    volatile u32 value;
    volatile u32 *slot;
    int v;
    unsigned short *q;
    int i;

    int Func_8015fb8(int src, int dst)
    {
        void (*fp)(void *, int);
        void *d;

        DMA3_COPY16((void *)(0x6000010 + (src & 0x3ff) * 32),
                    (void *)(0x6000000 + (dst & 0x3ff) * 32), 0x20);
        d = (void *)(0x600000c + (dst & 0x3ff) * 32);
        fp = Func_80008d4;
        fp(d, 0x14);
    }

    p = galloc_ewram(0xf, 0x12fc);
    slot = &value;
    *slot = 0;
    DMA3_SET((void *)slot, p, 0x85000000 | (0x12fc / 4));
    p[0xea3] = 1;
    q = (unsigned short *)(p + 0x12b6);
    v = 0x63;
    *q = v;
    p[0xea5] = 1;
    p[0xea7] = 0xf;
    *slot = 0xf000f000;
    DMA3_SET((void *)slot, p, 0x85000000 | (0x500 / 4));
    Func_8015ef4();
    StartTask(Func_80160fc, 0x90 << 3);
    Func_8017464(mode);
    Func_8015fb8(0xf013, 0x80);
    Func_8015fb8(0xf014, 0x81);
    Func_8015fb8(0xf015, 0x82);
    {
        int k = 4;
        for (i = 2; i >= 0; i--)
            p[0xda0 + i] = k;
    }
}
