/* Cluster Func_80f61e8..Func_80f61e8 extracted from goldensun/asm/rom_f6000/rom_f6008_c.s.
 *
 * Total .text for this TU = 208 bytes (= 0xd0). Never attempted before batch 275.
 * No pins, no flags.
 *
 * A per-channel palette cross-fade: step each 5-bit channel of the live palette one unit
 * toward the target loaded from a file.
 *
 * ================ A `(u16)` CAST BEFORE THE MASK SETTLES THE `_CONST_1f` CLASS ================
 *
 * `(u16)((c << 16) >> 21) & 0x1f` makes the mask a HImode pool entry (`*thumb_movhi_insn`,
 * `pool_range` 64), which is what forces the ROM's mid-function pool and its `b`-over-pool --
 * at ZERO extra instructions.
 *
 * docs/elevation.md calls the `(int)&_CONST_1f` reading a convincing FALSE LEAD -- "right
 * text, wrong bytes", because an SImode symbol has range 1020 and the pool moves to the end
 * of the function. And const.sym's own `_CONST_1f` entry lists `unsigned short t;
 * (t >> 5) & 0x1f` as a near miss "not close enough to count", because it prints `ldrh`.
 *
 * BATCH 79'S OWN FINDING SETTLES THAT OBJECTION: Thumb-1 has no PC-relative `ldrh`, so gas
 * assembles `ldrh rN, .L` to the identical halfword as `ldr`. The near miss was a match all
 * along and was rejected on a disassembly artefact. This file is the first validated
 * instance of the shape.
 *
 * SO NO `_CONST_1f` SYMBOL IS NEEDED for this class, and two things are worth re-screening
 * with the cast: src/non_matching/rom_f6000/80f6148.c, whose WHOLE residue is this pool (and
 * which now sits in the _c piece of this same split), and Func_80a2144.
 *
 * ONE MORE LEVER: COMMUTATIVE OPERAND ORDER ON A MASKED LOAD IS A REGISTER LEVER. `m & c`
 * gives the ROM's `mov r5, r10 / and r5, r3` -- the mask into the destination register --
 * where `c & m` costs one extra `mov` per channel, 97 lines against 95.
 *
 * NOTE FOR RE-SCREENING: tryc reports this as "ours 97, rom 96, 74 differ", a duplicate-label
 * artefact. objcmp is clean at 97 encodings and 1 relocation.
 */
#include "gba/types.h"
#include "dma.h"

extern void *GetFile(int id);

void Func_80f61e8(int id)
{
    u16 buf[0x40];
    u16 *pal;
    void *src;
    u32 c;
    u32 d;
    int r;
    int g;
    int b;
    int r2;
    int g2;
    int b2;
    int i;
    int m;

    pal = (u16 *)0x5000000;
    src = GetFile(id);
    DMA3_SET(src, buf, 0x84000020);
    i = 0;
    m = 0x1f;
    do {
        c = *pal;
        b = m & c;
        g = (u16)((c << 16) >> 21) & 0x1f;
        r = (u16)((c << 16) >> 26) & 0x1f;
        d = buf[i];
        b2 = m & d;
        g2 = (u16)((d << 16) >> 21) & 0x1f;
        r2 = (u16)((d << 16) >> 26) & 0x1f;
        if (b < b2)
            b++;
        else if (b > b2)
            b--;
        if (g < g2)
            g++;
        else if (g > g2)
            g--;
        if (r < r2)
            r++;
        else if (r > r2)
            r--;
        buf[i] = (r << 10) | (g << 5) | b;
        i++;
        pal++;
    } while (i != 0x40);
    DMA3_SET(&buf[1], (void *)0x5000002, 0x8000003f);
}
