/* Func_80d66cc -- asm/rom_c9000/rom_d6504_a_c_c_c.s
 *
 * BLOCKER: TRANSLATION-UNIT SIZE (pool placement). tryc says OK; the REAL
 * BUILD DISAGREES. This park exists mostly to record that.
 *
 * Builds a 0xa0-entry window table: for indices 8..0x87 it writes
 * clamp(gBuffer[0] - ewram_200fffa[i], 0, 0xf0), and 0xfff1 everywhere else,
 * then does the UnknownDMAPrefix() masking and a DMA0_SET of the table into
 * REG_WIN0H.
 *
 * IT SCREENS CLEAN. tryc reported `OK Func_80d66cc (50 lines)` on three
 * consecutive runs. Every instruction matches. Installed, the build FAILED
 * the checksum, reproducibly -- rebuilt from scratch twice, so not the ASLR
 * nondeterminism.
 *
 * 93,746 bytes of the ROM differ, starting at 0xc9004, well BEFORE the
 * function at 0x080d66cc. The differing values are mostly off by 2 and 4:
 * that is a pointer table at the head of the bank recording addresses that
 * moved, i.e. the whole bank shifted because this object is a different SIZE
 * from the assembly it replaced. The instructions are identical; the literal
 * pool is not.
 *
 * The generated pool is ONE block of nine words after the epilogue:
 *
 *     gBuffer, 0xfff1, ewram_2010082, ewram_200fffa, REG_DMA0SAD,
 *     0xc5ff, 0x7fff, REG_WIN0H, 0xa2600001
 *
 * The ROM SPLITS them: `0xfff1` sits in a mid-function pool at .Ld66f8 with
 * its own `.align 2, 0` and a `.pool`, ahead of the second half of the body,
 * and the rest follow later. batch 155 named the mechanism -- OPERAND MODE
 * CONTROLS POOL PLACEMENT VIA pool_range, HImode having a 64-byte range and
 * SImode 1020 -- and the fix direction is to force the 0xfff1 to be dumped
 * early rather than pooled with everything else. Reaching it through an
 * `int` local is what SImode-ifies a constant, and this park already does
 * that (`fill`), which is presumably why it ended up in the single late pool.
 * The untried direction is the opposite one: write it as a bare literal into
 * the halfword store so it is HImode and forces the early dump.
 *
 * THE PROCESS POINT, which is the reusable part:
 *
 *   tryc compares INSTRUCTIONS. It cannot see pool placement, and it says so
 *   -- it printed "the reference keeps its literal pool INSIDE the function
 *   ... VERIFY WITH make compare" on every one of those three OK runs. An OK
 *   carrying that warning is not a match, it is a match of the instruction
 *   stream. This is the mirror image of Func_801fda8 in the same batch, where
 *   the same warning accompanied a 6-DIFFERING report on a function that was
 *   byte-identical. On a pool-carrying function tryc is wrong in both
 *   directions and only `make compare` decides.
 *
 * WHAT WAS CORRECT and is kept below, since the instruction stream matches:
 * the unsigned range guard `(unsigned)(i - 8) <= 0x7f`; the birth order that
 * puts gBuffer and the 0xfff1 constant ahead of the two pointers (11 -> 8
 * differing); and `i = 0` assigned BEFORE the pointer assignments with an
 * empty `for` init, which flipped the r0/r1 allocation and closed the last 8.
 * That last one is worth carrying: permuting the DECLARATION order did
 * nothing, the ASSIGNMENT position decided the register.
 */
#include "dma.h"

extern unsigned short gBuffer[];
extern unsigned char ewram_200fffa[];
extern unsigned short ewram_2010082[];

void Func_80d66cc(void)
{
    unsigned short *g;
    int fill;
    unsigned short *out;
    unsigned char *sub;
    int i;
    int v;

    g = gBuffer;
    fill = 0xfff1;
    i = 0;
    out = ewram_2010082;
    sub = ewram_200fffa;
    for (; i != 0xa0; i++) {
        if ((unsigned int)(i - 8) <= 0x7f) {
            v = g[0] - sub[0];
            if (v < 0)
                v = 0;
            if (v > 0xf0)
                v = 0xf0;
            *out = v;
        } else {
            *out = fill;
        }
        out++;
        sub++;
    }
    UnknownDMAPrefix();
    DMA0_SET(ewram_2010082, (void *)&REG_WIN0H, 0xa2600001);
}
