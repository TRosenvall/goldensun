/* DecompressIcon (AllocIconBuffer) -- 0x08021be0 -- NON-MATCHING.
 * Blocker class: CONSTANT FOLDING of a shift the ROM performs at runtime.
 * 24 lines against the ROM's 28, FOUR SHORT, 27 differing.
 *
 * The buffer size 0x278 is used twice: as the allocation size, and shifted
 * right two to make the DMA word count. The ROM loads 0x278 into r5, holds it
 * across the galloc_iwram call, and shifts it AFTER:
 *
 *     ldr r5, =0x278 / ... / bl galloc_iwram / lsr r5, #2 / orr r2, r5
 *
 * gcc folds `0x278 >> 2` to 0x9e at compile time -- correctly, since the value
 * is a literal -- and never needs the register, so the four instructions that
 * hold and shift it are absent.
 *
 * Tried:
 *   - `0x84000000 | (size >> 2)` written out:   27 differing
 *   - `DMA3_COPY(src, buf, size)`, which divides by four inside the macro:
 *     identical output. The fold happens either way.
 *
 * Nothing at C level asks for a compile-time-constant shift to be performed at
 * runtime, so this needs either a non-constant source for the size -- which
 * would mean the original had it in a variable this decompilation has not
 * identified -- or a compiler that does not fold. Recorded as unresolved
 * rather than assigned to either.
 *
 * SOLVED HERE and worth the park on its own: the indirect call. The ROM's
 * `bl _call_via_r3` is an ordinary function-pointer local, and the form below
 * produces it. See docs/elevation.md, "Indirect calls: _call_via_r3 is a
 * solved shape". 149 remaining .s files use that construct and it had been
 * steered around as if it were a compiler-support routine.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned char Func_8015afc[];
extern unsigned char gPtrs[];

void DecompressIcon(char *rec)
{
    int size;
    void *buf;
    void (*fp)(void *, char *);

    size = 0x278;
    buf = galloc_iwram(0x31, size);
    DMA3_SET(Func_8015afc, buf, 0x84000000 | (size >> 2));
    fp = *(void (**)(void *, char *))(gPtrs + 0xc4);
    fp(*(void **)(rec + 0x604), rec);
    gfree(0x31);
}
