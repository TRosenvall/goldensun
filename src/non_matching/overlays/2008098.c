/* OvlFunc_916_2008098  --  0x02008098, asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a.s
 * and its byte-identical twin OvlFunc_947_2008cc0  --  0x02008cc0,
 * asm/overlays/rom_7d0e88/ovl_314_a_c_c_c_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a.s
 *
 * BLOCKER CLASS: loop-invariant motion of ADDRESS CONSTANTS, and the register
 * pressure it causes. Status: 88 lines against the ROM's 84.
 *
 * A tilemap blit. Walks a w x h window of a 128-int-wide buffer and, for each
 * cell, uses the low twelve bits as an index into two parallel eight-byte-stride
 * tables in EWRAM, writing one word to 0x6002800 + i*4 and the other to
 * 0x6002840 + i*4. The destination index wraps both axes to sixteen and offsets
 * the row by `page << 4`.
 *
 * THE READING IS RIGHT -- every loop bound, both `& 0xf` wraps, the `<< 3`
 * table stride, the `0x80 - w` row advance and the two VRAM bases all appear on
 * both sides. What differs is how many constants live in registers.
 *
 * THE ROM RE-LOADS THREE POOL CONSTANTS EVERY INNER ITERATION:
 *
 *     ldr r6, =ewram_2020000  ...  ldr r6, =ewram_2020004  ...  ldr r1, =0x6002840
 *
 * and holds only 0xfff, 0xf and 0x6002800 across the loop. gcc hoists all five
 * out, which costs five live values, which spills `w`, the row limit, the row
 * stride and `page << 4` to the stack -- a 0x14 frame against the ROM's 8 --
 * and the extra loads and stores are the four surplus instructions.
 *
 * TRIED AND MEASURED:
 *
 *   `*(int *)(0x6002800 + i)` with byte-array externs (this file)   88 / 74
 *   `int` array externs indexed `[(t & 0xfff) * 2]`                 89 / 75
 *   `((int *)0x6002800)[i >> 2]`                                    89 / 75
 *   `i += 0x6002800` once, then `*(int *)i` and `*(int *)(i + 0x40)` 83 / 70
 *
 * The last is the closest in size and no closer in shape: it removes one
 * hoisted constant and gcc immediately hoists something else.
 *
 * `-fno-loop-optimize` DOES NOT EXIST in this cc1 -- `Unrecognized option`. So
 * the pass cannot be switched off to confirm the diagnosis from the other side,
 * and no source spelling stops gcc hoisting an address it can prove invariant.
 *
 * NOT A REGISTER PERMUTATION. Ours needs four more stack slots than the ROM's,
 * so this is not the transposition class from docs/elevation.md -- it is a
 * different decision about what belongs in a register at all. Two functions
 * come with it.
 */

extern unsigned char gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void OvlFunc_916_2008098(int col, int row, int w, int h, int page, int x0, int y0)
{
    unsigned int *src;
    int x, y, i;
    unsigned int t;

    src = (unsigned int *)gBuffer + (col + (row << 7));
    for (y = y0; y < y0 + h; y++) {
        for (x = x0; x < x0 + w; x++) {
            t = *src++;
            i = ((((y & 0xf) + (page << 4)) << 5) + (x & 0xf)) << 2;
            *(int *)(0x6002800 + i) = *(int *)(ewram_2020000 + ((t & 0xfff) << 3));
            *(int *)(0x6002840 + i) = *(int *)(ewram_2020004 + ((t & 0xfff) << 3));
        }
        src += 0x80 - w;
    }
}
