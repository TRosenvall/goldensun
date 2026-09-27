/* Func_8010ff0 -- NON-MATCHING, 101 encodings of 112.  ref 240 bytes / ours 224,
 * ref 112 instructions / ours 104 -- EIGHT SHORT, so the 101 is mostly the shift.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/8010ff0.c \
 *     asm/rom_9000/rom_108e4_c.s --func Func_8010ff0
 *
 * Landing needs the same text/data split as its file-mates -- see 8010e14.c's header.
 *
 * USEFUL POSITIVES, all reproduced: the prologue through the DISPCNT read matches;
 * `UnknownDMAPrefix()` and `DMA0_SET()` from include/dma.h ARE exactly this function's
 * prelude and DMA (the ROM even derives the DMA0_SET destination as REG_DMA0SAD - 0x90 =
 * REG_BG2PA by move2add, which is why dma.h's comment about reusing the already-loaded base
 * is correct); the `ldmia r1!, {r3}` plus later `ldr r1,[r1]` pair is a walking
 * `unsigned char **p = &iwram_3001e6c; view = *p++ ...; state = *p;`; and the
 * `neg/orr/lsr #31/lsl #1` tail is `(b != 0) << 1`, reproduced exactly.
 *
 * TWO BLOCKERS:
 *
 * 1. THE 8-WORD COPY TO REG_BG2PA..REG_BG2Y.  A `struct {u32 w[8];}` assignment goes
 *    through the ARM `movstr` pattern and emits THREE-register `ldmia r3!,{r5,r6,r7}`
 *    bursts; the ROM has EIGHT single-register `ldmia r0!,{r3}` / `stmia r4!,{r3}` pairs
 *    with plain `str` at each end -- i.e. `move_by_pieces` with auto-increment on both
 *    sides, or eight source-level `*d++ = *s++;`.  NOT YET TESTED, and it is the first
 *    thing to try.
 *
 * 2. `ldrsh` WHERE THE ROM HAS `ldrh` for the three state+0x100/0x102/0x104 halfwords.
 *    This goes through `extendhisi2` (hence the manufactured zero index register); the ROM's
 *    is a plain `*thumb_movhi_insn` HImode load (arm.md:4318, which always prints `ldrh`).
 *    The type spelling that keeps it in HImode WITHOUT a sign-extend is not yet found.
 *    Related and already correct: the ROM RE-LOADS state+0x104 right after storing to it,
 *    which this candidate reproduces.
 */
#include "dma.h"

extern unsigned char *iwram_3001e6c;
extern unsigned int iwram_3001e40;

struct Affine {
    unsigned int w[8];
};

void Func_8010ff0(void)
{
    unsigned char **p;
    unsigned char *view;
    unsigned char *state;
    unsigned int *src;
    short d;
    int mode;
    unsigned short a;
    unsigned short b;
    unsigned short c;

    p = &iwram_3001e6c;
    view = *p++ + (0xc8 << 4);
    d = REG_DISPCNT & 0xfff8;
    state = *p;
    UnknownDMAPrefix();
    if (view != 0) {
        src = (unsigned int *)(view + (iwram_3001e40 & 1) * 0x1400);
        *(struct Affine *)REG_ADDR_BG2PA = *(struct Affine *)src;
        DMA0_SET(src + 8, (void *)REG_ADDR_BG2PA, 0xa6600008);
    }
    a = *(unsigned short *)(state + 0x100);
    *(unsigned short *)(state + 0x104) = a;
    b = *(unsigned short *)(state + 0x102);
    *(unsigned short *)(state + 0x106) = b;
    c = *(unsigned short *)(state + 0x104);
    mode = 0;
    if (c <= 0xc7) {
        mode = (b != 0) << 1;
        if (c <= b) {
            mode = 0;
            if (c == 0)
                mode = 2;
        }
    }
    REG_DISPCNT = d | mode;
    *(unsigned short *)(state + 0x108) = 0;
}
