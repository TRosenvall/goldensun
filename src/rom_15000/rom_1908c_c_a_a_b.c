/* Cluster DecompressString..DecompressString extracted from goldensun/asm/rom_15000/rom_1908c_c_a_a.s.
 *
 * Total .text for this TU = 240 bytes (= 0xf0). Never attempted before batch 276.
 * No pins, no flags. Requires _SIZE_8015430; see size.sym.
 */
/* DecompressString -- 0x080196c4
 *
 * Runs the Huffman string decoder over one string id and expands it into the
 * caller's halfword buffer, stopping when fewer than the next token's worth of
 * slots remain. The reader lives in RAM: if the gPtrs slot at +0xc8 is still
 * null the function allocates 0x140 bytes under tag 0x32, DMA3-copies the ARM
 * routine Func_8015430 into it and frees it again on the way out. Same
 * copy-the-routine-into-RAM trick as DecompressIcon / LoadIcon.
 *
 * THE SIZE IS A SYMBOL, and it is the SIZE OF THE ROUTINE COPIED -- 0x140 is
 * exactly `Func_8015570 - Func_8015430` in asm/rom_15000/rom_15430.s, the same
 * arithmetic that identifies every entry in size.sym. Criterion 1 of const.sym's
 * tell is met: gcc BUILDS 320 as `mov r1, #0xa0 / lsl r1, #1` (it is a
 * thumb-shiftable constant), while the ROM pools it. Criterion 2 is met too --
 * eight literal spellings (`0x140`, `320`, `(unsigned)320`, `0xa0 << 1`,
 * `0x50 << 2`, `sizeof(int) * 80`, `4 * 80 + 0`, `0x13f + 1`) all give the same
 * `mov`/`lsl` pair, and worse: a compile-time-constant size lets gcc fold the
 * whole DMA control word to one pooled `0x84000050`, which deletes the shared
 * size register and takes the function from 1 differing to 98.
 *
 * THE TWO `-1`s IN THE case-0xe ARM AND THE ONE IN THE 8..0xc/0xf ARM ARE SPELLED
 * DIFFERENTLY, and that is the whole of the register allocation. The ROM holds
 * 0xffff in r9 across the loop and uses it twice, then materialises a SECOND
 * 0xffff from the pool into r2 inside the other arm. Writing `+ 0xffff` at all
 * three sites gives loop.c one invariant with THREE references: it is hoisted
 * once, the third pool load disappears, and -- because three in-loop references
 * outrank two -- the constant's allocno is now processed before `p`'s, so r9 and
 * r10 swap for the whole function (41 differing). `- 1` at the single site keeps
 * it a one-use CONST_INT that reload rematerialises in place, which restores
 * both the pool load and the r10/r9 order in one change.
 *
 * `gPtrs` is read TWICE from a NAMED base local. The named base is what keeps
 * `ldr r3, =gPtrs / add r3, #0xc8` out of a folded `=gPtrs+200` pool word, and
 * the two reads are what let gcse insert the reload INSIDE the allocating arm
 * (`mov r2, r8 / ldr r3, [r2]`) while the fall-through path reuses the first
 * value (`mov r3, r10`). Caching the pointer in one local removes both.
 *
 * `*out = 0` through the `unsigned short *` pools its zero (`ldr r3, =0x0`) --
 * the plain HImode-literal behaviour, no `int` intermediate wanted here.
 */
#include "dma.h"

typedef unsigned int (*Reader)(void *);

extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned char gPtrs[];
extern unsigned char Func_8015430[];
extern void HuffStr_Start(void *a, void *b);
extern int _SIZE_8015430;

void DecompressString(void *str, unsigned short *out, int n)
{
    unsigned char st[12];
    unsigned char *g;
    Reader p;
    Reader read;
    unsigned int c;
    unsigned int size;

    g = gPtrs;
    p = *(Reader *)(g + 0xc8);
    if (p == 0) {
        unsigned char *buf;
        size = (unsigned int)(int)&_SIZE_8015430;
        buf = galloc_iwram(0x32, size);
        DMA3_COPY(Func_8015430, buf, size);
    }
    read = *(Reader *)(g + 0xc8);
    HuffStr_Start(st, str);
    while ((c = read(st)) != 0) {
        switch (c) {
        case 0xe:
            n -= 3;
            if (n <= 0) goto done;
            *out++ = c;
            *out++ = read(st) + 0xffff;
            *out++ = read(st) + 0xffff;
            break;
        case 8: case 9: case 0xa: case 0xb: case 0xc: case 0xf:
            n -= 1;
            if (n <= 0) goto done;
            *out++ = c;
            *out++ = read(st) - 1;
            break;
        default:
            n -= 1;
            if (n <= 0) goto done;
            *out++ = c;
            break;
        }
    }
done:
    if (p == 0)
        gfree(0x32);
    *out = 0;
}
