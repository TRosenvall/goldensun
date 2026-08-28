/* Func_80b0840 (UploadShopGraphics) -- NON-MATCHING.
 * Blocker class: THE MACRO'S PINNED REGISTER IS ALREADY CORRECT.
 *
 * 29 lines against the ROM's 30 -- ours is one instruction SHORT, and the
 * missing instruction is a `mov r0, r4` that does nothing.
 *
 * The function issues two DMA3 transfers that READ FROM THE SAME SOURCE into
 * two different destinations. DMA3_SET in include/dma.h pins its arguments:
 *
 *     register const void *_src __asm__("r0") = src;
 *
 * After the first transfer r0 still holds that source -- the inline asm
 * clobbers "memory" but not r0 -- so for the second call gcc sees the pinned
 * register already correct and emits nothing. The ROM re-issues `mov r0, r4`
 * anyway. Every later difference is that one-instruction shift.
 *
 * WHY THIS IS NOT THE SAME AS THE OTHER DOUBLE-DMA FUNCTIONS. The tree already
 * matches several: src/overlays/rom_7a4370/ovl_30_c_c_c_c_c_c_b.c calls
 * DMA3_COPY twice in a row and is byte-exact. Its two calls have DIFFERENT
 * sources, so gcc has to reload r0 and the elision never arises. The
 * precondition for this park is specifically two transfers sharing a source.
 *
 * TWO THINGS WERE SOLVED and are kept in the source below.
 *
 *   1. SOURCE ORDER. Reading iwram_3001ebc[0] before computing the base+0xe00
 *      pointer is what gives the ROM's `ldr r1, [r3] / add r4, r5, r2` order.
 *      The other way round it is 14 differing from line 5; this way the first
 *      difference is at line 18 and everything before it is exact.
 *   2. iwram_3001ebc IS AN ARRAY OF POINTERS here, not a pointer. The ROM
 *      reads [r3] and [r3, #0x14], which is elements 0 and 5.
 *
 * Tried and byte-identical to each other (all 12 differing):
 *   - the shared source in one named local, passed to both calls
 *   - the expression `base + (0xe0 << 4)` written out inline at both calls
 *   - a second named local assigned the same expression before the second call
 *
 * All three are the same value and gcc knows it. Nothing at the C level can
 * ask for a redundant register copy.
 *
 * NOT TRIED, ON PURPOSE: adding "r0" to DMA3_SET's clobber list would force
 * the reload, but that macro is what makes several other functions match, and
 * changing it to suit this one risks them. If this is ever revisited, measure
 * the whole corpus of DMA3_SET users before touching include/dma.h.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *iwram_3001ebc[];
extern void _Func_8091200(int a, int b);
extern void _Func_8091254(int n);

void Func_80b0840(int index)
{
    char *base;
    char *d;

    base = iwram_3001ebc[5];
    d = iwram_3001ebc[0] + 0x236;
    DMA3_SET(base + (0xe0 << 4), d, 0x84000150);
    DMA3_SET(base + (0xe0 << 4), base + (0xe0 << 2), 0x840002a0);
    _Func_8091200(index, 1);
    _Func_8091254(0x10);
}
