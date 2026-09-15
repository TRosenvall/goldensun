/* DeleteSprite -- 0x0800bdd4, split out of asm/rom_9000/rom_b798_c_c_c_a.s.
 *
 * Releases a sprite: drops its palette slot unless a flag says not to, deletes
 * its four layers, and zeroes the record.
 *
 * EXACT ON THE FIRST CANDIDATE, and it is the batch-265 loop result paying off
 * immediately. The ROM's layer loop reads
 *
 *     mov r6, #3 / ldmia r5!, {r0} / sub r6, #1 / bl / cmp r6, #0 / bge
 *
 * -- a counter initialised to 3, counting DOWN, with a post-increment load. Both
 * halves are the optimiser's, not the source's: `for (i = 0; i < 4; i++)` over
 * `s->layers[i]` is what produces it. check_dbra_loop reverses the ascending
 * loop into the countdown and strength_reduce turns the index into the
 * `ldmia r5!` walking pointer. Writing either of those back into C is what
 * batch 265 recorded as the trap, so the loop was written ascending on the first
 * try and matched with no iterations.
 *
 * `bge` rather than `bne` is the tell that this is FOUR iterations and not
 * three: the counter reaches 0, tests, and runs the body once more.
 *
 * The tail is DMA3_CLEAR(s, 0x38) -- the ROM's count word 0x8500000e is
 * 0x85000000 | (0x38 / 4), and 0x38 is also exactly sizeof this struct, which is
 * what fixes the four layer pointers at 0x28..0x37 and closes the layout.
 *
 * EXACT: 76 bytes, 34 encodings, 2 relocations, measured three times, and clean
 * on tools/tryc.py as well.
 */
#include "gba/types.h"
#include "dma.h"

struct Sprite {
    u8 pad00[0x1c];
    u8 f1c;
    u8 f1d;
    u8 pad1e[10];
    void *layers[4];
};

extern void Func_8003f3c(int n);
extern void DeleteSpriteLayer(void *p);

void DeleteSprite(struct Sprite *s)
{
    int i;

    if (s != NULL) {
        if ((s->f1d & 1) == 0)
            Func_8003f3c(s->f1c);
        for (i = 0; i < 4; i++)
            DeleteSpriteLayer(s->layers[i]);
        DMA3_CLEAR(s, 0x38);
    }
}
