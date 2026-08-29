/* Cluster Func_809bb34..Func_809bb34 extracted from goldensun/asm/rom_8a000/rom_9b698_c_c.s.
 *
 * Slotted between rom_9b698_c_c_a.o and the rest of stage1.ld.
 *
 * Deletes the sprite a struct points at, if any, then zeroes the struct.
 *
 * FIRST USE OF DMA3_CLEAR IN THIS TREE. include/dma.h has carried it since the
 * tree was adopted and nothing had used it; tools/find_solved_shape.py reports
 * no elevated .c producing a `0x85000...` count word, which is what identified
 * it as the unused one of the family rather than something needing a new
 * construct. The size argument is bytes -- 0x48 here, giving the ROM's
 * 0x85000012.
 *
 * The DeleteSprite argument is the pointer READ OUT of the struct, not the
 * struct itself; the ROM loads it once into r0 for both the test and the call.
 */
#include "dma.h"
extern void _DeleteSprite(void *s);

void Func_809bb34(void *p)
{
    if (*(void **)p != 0)
        _DeleteSprite(*(void **)p);
    DMA3_CLEAR(p, 0x48);
}
