/* Func_8012388 (0x08012388) -- NON-MATCHING.
 * Blocker class: gcc CONSTANT-FOLDS a size the ROM keeps in a register.
 *
 * 34 lines against 39. The allocation size is used twice -- once as
 * `galloc_iwram`'s second argument and once as `DMA3_COPY`'s count -- and the
 * ROM keeps it in r5 and computes the count word at RUNTIME:
 *
 *     rom    ldr r5, =0x27c / ... / lsr r5, #0x2 / lsl r2, #0x18 / orr r2, r5
 *     ours   mov r1, #0x9f / lsl r1, #0x2 ... ldr r2, =0x8400009f
 *
 * `DMA3_COPY(src, dst, size)` builds `0x84000000 | (size / 4)`. With `size` a
 * local initialised to a literal, gcc folds the whole word; the ROM's compiler
 * did not. This is the batch-174 shape -- a fold the ROM declined -- but the
 * recorded fix (separate the constants into different statements) does not
 * apply, because here it is ONE constant that must stay a variable.
 *
 * MEASURED (rom 39 lines):
 *   `n = 0x27c;` then `galloc_iwram(0x31, n)` and `DMA3_COPY(..., n)`  33, 38
 *   the same plus `g = gPtrs; g += 0xc4;` for the function-table base   34, 38
 *
 * The gPtrs base is right and is kept: written `gPtrs + 0xc4` it folds to a
 * pooled `gPtrs+196` where the ROM has `ldr =gPtrs / add r3, #0xc4`.
 *
 * WHAT IS RIGHT: r8 and r10 holding both arguments across the calls, with no
 * lever; the `_call_via_r4` indirect call through a function-pointer local; and
 * `Func_8009e7c` used as a DMA source address.
 *
 * NEXT: a way to keep a local constant unfolded, which nothing in the notebook
 * currently offers.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char ewram_201c000[];
extern unsigned char ewram_203c000[];
extern unsigned char gPtrs[];
extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void Func_8009e7c(void);

typedef void (*Fn)(int a, int b, void *c, void *d);

void Func_8012388(int a, int b)
{
    unsigned char *e;
    unsigned char *p;
    unsigned char *g;
    Fn fn;
    int n;

    e = ewram_201c000;
    n = 0x27c;
    p = galloc_iwram(0x31, n);
    DMA3_COPY(Func_8009e7c, p, n);
    g = gPtrs;
    g += 0xc4;
    fn = *(Fn *)g;
    e += 0x80 << 5;
    fn(a, b, ewram_203c000, e);
    gfree(0x31);
}
