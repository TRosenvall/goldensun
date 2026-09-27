/* Func_80f0538  --  0x080f0538, split out of asm/rom_f0000/rom_f0254_c.s (StartGS1Credits
 * stays in _a.s; Func_80f0614, 80f0678, 80f07f0 and the .rodata in _c.s; one
 * data label was exported for the split). Matched from scratch.
 *
 * - `7 & pos` with the constant FIRST puts the result in r4.
 * - The tile index in ONE expression, `((u16)((s16)pos / 8) & 0x1f) * 24`,
 *   gives the HImode pool constant.
 * - The inner loop counts UP; gcc reverses it itself.
 * - x is computed from the counter, `((j * 32 + 24) << 16)`, not kept as a
 *   running `x += 0x200000`: the running variable put the loop setup in the
 *   wrong order (8-11 differing) and 60 statement permutations were inert.
 *   When a loop's setup order is wrong and permutation does nothing, derive
 *   the running value from the counter instead.
 */
#include "gba/types.h"
#include "dma.h"

extern u16 ewram_2004c00;
extern s16 ewram_2004c04;
extern u32 *ewram_2004c0c;
extern u32 iwram_3001800;

void Func_80f0538(void)
{
    u32 pos = ewram_2004c00;
    int sub = 7 & pos;
    int tile = ((u16)((s16)pos / 8) & 0x1f) * 24;
    u32 *p;
    u32 *q;
    int row, j;

    p = ewram_2004c0c + 0x30;
    for (row = 0; row <= 15; row++) {
        for (j = 0; j < 6; j++) {
            q = p;
            *q++ = (16 - sub + row * 8) | ((j * 32 + 24) << 16) | 0x40004000;
            *q = tile;
            tile += 4;
            p += 2;
            if (tile == 0x300)
                tile = 0;
        }
    }
    DMA3_SET(ewram_2004c0c, (void *)0x7000000, 0x84000100);
    if (ewram_2004c04 == 0 && (iwram_3001800 & 3) == 0)
        ewram_2004c00++;
}
