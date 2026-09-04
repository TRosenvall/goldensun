/* InitSprites -- 0x0800bb20
 *
 * Allocates the two sprite tables -- 0xe00 bytes under tag 4 and 0x600 under
 * tag 3, from ewram when the mode argument is 3 and iwram otherwise -- zeroes
 * both by DMA, uploads the shared OBJ tile range, then copies the 0x7c-byte
 * ARM helper Func_800a418 into a scratch buffer to run it there.
 *
 * A THIRD USER OF size.sym, and the one that shows the space is real. Its size
 * is 0x7c, the same VALUE as _SIZE_8015e10 in batch 216 -- but a different
 * routine, in a different file, pooled in the function that copies its own.
 * `_SIZE_800a418` and `_SIZE_8015e10` are separate symbols for that reason; one
 * shared `_CONST_7c` would have matched the bytes and asserted something false.
 * Here 0x7c fits an eight-bit `mov` and the ROM pools it anyway, which is
 * const.sym's criterion 1 met literally rather than by extension.
 *
 * THE TWO DMA CLEARS SHARE ONE STACK WORD, the same frame-size tell as
 * src/rom_8a000/rom_8d9a4_c_c_c_a_c_c_b.c: `sub sp, #0x4` means ONE `u32`, so
 * the source cannot use dma.h's DMA3_CLEAR twice (each inline declares its
 * own). One local, its address in a local PINNED to r4, and two DMA3_SET calls.
 * The stored zero is pinned to r5 for the same reason it was there -- the ROM
 * keeps it in a callee-saved register across both writes.
 *
 * DO NOT PIN THE SIZE. r5 carries the zero first and the 0x7c size afterwards,
 * so the one-variable-two-ranges reading says to declare both on r5. That is
 * WRONG here: with `n` also pinned to r5 the allocator puts the DMA control
 * word in r4 and shifts the size out of place (`lsr r2, r5, #0x2` against the
 * ROM's destructive `lsr r5, #0x2`), costing four lines. Leaving `n` unpinned
 * lets gcc pick r5 by itself and the whole transfer comes out exact. Two
 * competing pins on one register is worse than one pin and a free choice.
 *
 * UploadSpriteGFX fills r0 before r1 where the ROM fills r1 first; a local
 * pinned to r1 fixes it. A barrier on that local was tried as well and is
 * WORSE (3 lines against 1) -- batch 215's "pins first, barrier only if the
 * pairs re-fuse" holding in the direction where the barrier is not needed.
 */
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern unsigned char *galloc_iwram(int tag, int size);
extern void LoadSpritePalette(void);
extern void UploadSpriteGFX(int a, int n, void *p);
extern unsigned char L12f20[] __asm__(".L12f20");
extern unsigned char Func_800a418[];
extern int _SIZE_800a418;

void InitSprites(int mode)
{
    unsigned char *p7;
    unsigned char *p6;
    unsigned char *q;
    u32 value;
    register u32 *v __asm__("r4");
    register int z __asm__("r5");
    unsigned int n;
    register int sz __asm__("r1");

    if (mode == 3) {
        p7 = galloc_ewram(4, 0xe0 << 4);
        p6 = galloc_ewram(3, 0xc0 << 3);
    } else {
        p7 = galloc_iwram(4, 0xe0 << 4);
        p6 = galloc_iwram(3, 0xc0 << 3);
    }
    LoadSpritePalette();
    z = 0;
    v = &value;
    *v = z;
    DMA3_SET(v, p7, 0x85000380);
    *v = z;
    DMA3_SET(v, p6, 0x85000180);
    sz = 0x80;
    UploadSpriteGFX(0x5d, sz, L12f20);
    n = (unsigned int)(int)&_SIZE_800a418;
    q = galloc_iwram(0x35, n);
    DMA3_COPY(Func_800a418, q, n);
}
