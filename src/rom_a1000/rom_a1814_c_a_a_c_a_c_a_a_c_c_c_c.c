/* Cluster Func_80a2144 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_c_c.s.
 *
 * LoadMenuPalette. DMA3-copies the sixteen colours at 0x50001E0 into the OBJ palette
 * bank selected by the argument, queues a second eight-halfword copy for the next
 * VBlank, then brightens that bank's colour 4 by nine steps on each of red, green and
 * blue, clamped at 31. Whole-file conversion: the piece holds one function and no data.
 * No pins, no flags.
 *
 * ================ ONE CHARACTER: A `(u16)` CAST BEFORE THE GREEN MASK ================
 *
 * The ROM masks two channels with the same value and POOLS ONE OF THEM while building
 * the other with an eight-bit immediate -- `ldr r3, .La218c` beside `mov r0, #0x1f`.
 * gcc-2.96 never pools a constant an eight-bit `mov` can build, so const.sym's header
 * reads a pooled small value as a SYMBOL tell, and an entry `_CONST_1f` was added on
 * that reading.
 *
 * It is not a symbol. It is a MODE. `(u16)(t >> 21) & 0x1f` makes the mask an HImode
 * pool entry -- `*thumb_movhi_insn` with `pool_range` 64 (arm.md:4318, :4353) -- which
 * sorts ahead of every SImode fix in `add_minipool_forward_ref` (arm.c:4820) and so
 * forces `arm_reorg` to dump a mid-function pool with the ROM's `b` over it, at zero
 * extra instructions. Thumb-1 has no pc-relative halfword load, so gas assembles the
 * pattern's `ldrh rN, .L` as the ROM's `ldr rN, [pc, #imm]`.
 *
 * The SImode literal beside it is REQUIRED, not incidental: it is what keeps the third
 * channel's mask an immediate. Cast all three and the function still does not match.
 *
 * THIS WAS WRITTEN DOWN AND NEVER ACTED ON, which is the reusable lesson. Both
 * const.sym's `_CONST_1f` entry and the landed src/rom_f6000/rom_f6008_c_a_d.c name
 * THIS FUNCTION in writing as the thing to re-screen with the cast. The park went on
 * accumulating literal spellings for several more batches. When a landed file leaves a
 * standing instruction naming a park, read it before measuring anything.
 *
 * AND `_CONST_1f` NOW HAS NO BENEFICIARY. Its entry rests on this function's pooled
 * mask and says so; with the cast there is nothing left that needs the symbol. The
 * entry's own text already carries the batch-276 correction. Retiring it is an owner
 * call, not this landing's, but the evidence for it is now complete.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80a2144(int bank)
{
    unsigned short *pal;
    unsigned short c;
    unsigned int r, g, b;
    unsigned int t;

    pal = (unsigned short *)((bank << 5) + (0xa0 << 19));
    DMA3_SET((void *)0x50001e0, pal, 0x80000010);
    DMA3_SET((void *)0x50001e0, pal, 0x84000008);
    c = pal[4];
    t = (unsigned int)c << 16;
    b = t >> 26;
    g = (u16)(t >> 21) & 0x1f;
    r = 0x1f & c;
    b += 9;
    if (b > 0x1f)
        b = 0x1f;
    g += 9;
    if (g > 0x1f)
        g = 0x1f;
    r += 9;
    if (r > 0x1f)
        r = 0x1f;
    pal[4] = (b << 10) | (g << 5) | r;
}
