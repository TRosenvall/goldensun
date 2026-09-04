/* DecompressIcon -- 0x08021be0
 *
 * Allocates a 0x278-byte scratch under tag 0x31, DMA3-copies the nibble decoder
 * at Func_8015afc into it, calls the decoder through the gPtrs slot at +0xc4
 * with the caller's buffer, and frees the scratch. The run-a-routine-from-RAM
 * trick; LoadIcon in rom_19ebc_c.s is the same shape and is not yet elevated.
 *
 * THREE THINGS, none of them arithmetic:
 *
 *  - THE SIZE IS A SYMBOL, and it is the SIZE OF THE DECODER. 0x278 is exactly
 *    `Func_8015d74 - Func_8015afc`: rom_15430.s holds the two back to back, so
 *    the number is how many bytes of ARM code get copied. That explains the
 *    pool -- a size expressed as a symbol difference is a link-time value with
 *    no constant for gcc to build. Writing the difference in C does NOT work,
 *    though: gcc cannot fold two external symbols and emits two pool words and
 *    a runtime `sub`. A single symbol carrying the value is what reproduces the
 *    ROM. Eight literal spellings were measured and all eight build the number
 *    with `mov #0x9e / lsl #2`; the ROM itself builds the same value at four
 *    unrelated sites and pools it only here and in LoadIcon, which is the
 *    counterexample that makes the pool a tell. See `_SIZE_8015afc` in size.sym
 *    for the full argument, and LoadIcon in rom_19ebc_c_b.c for the second
 *    user of the same id space.
 *
 *  - THE DMA WRITE IS include/dma.h's DMA3_COPY, unchanged. Its
 *    `0x84000000 | (size / 4)` is exactly the ROM's
 *    `mov r2, #0x84 / lsr r5, #2 / lsl r2, #24 / orr r2, r5`, and the size has
 *    to reach it as a VARIABLE -- passing the literal lets gcc fold the whole
 *    count to one pooled `0x8400009e` and the shared register disappears.
 *
 *  - THE FUNCTION POINTER IS LOADED AFTER ITS ARGUMENTS. Fetching it into a
 *    named local first puts `ldr r3, [r3]` ahead of the argument load and picks
 *    r2 for the pointer, so the call comes out as `_call_via_r2`. Writing the
 *    whole thing as one call expression evaluates the arguments first and gives
 *    the ROM's `_call_via_r3`.
 *
 * The barrier on the parameter copy is ordering only: without it gcc issues the
 * pool load before `mov r6, r0`. gPtrs needs the array idiom for the usual
 * reason -- inlined, base and offset fold into a single `=gPtrs+196` pool word.
 */
#include "dma.h"

extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned char gPtrs[];
extern int _SIZE_8015afc;
extern unsigned char Func_8015afc[];

void DecompressIcon(unsigned char *arg)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *g;
    unsigned int n;

    a = arg;
    __asm__ volatile ("" : : "r" (a));
    n = (unsigned int)(int)&_SIZE_8015afc;
    p = galloc_iwram(0x31, n);
    DMA3_COPY(Func_8015afc, p, n);
    g = gPtrs;
    (*(void (**)(int, unsigned char *))(g + 0xc4))(*(int *)(a + 0x604), a);
    gfree(0x31);
}
