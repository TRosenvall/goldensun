/* Func_8006240 (0x08006240) -- NON-MATCHING. FIRST DRAFT, not a worked park.
 * NON-MATCHING: 134 encodings of 130 differ (objcmp), ours 135 -- the counts
 * DISAGREE, so 134 is not a distance: size is 296 against 280 and relocations
 * differ too.
 * Blocker class: not yet established. Two named defects, both diagnosed from a
 * line-for-line objdump diff, neither yet fixed:
 *
 *  1. THE SIO ADDRESSES MUST BE TWO POOL WORDS, NOT ONE PLUS AN OFFSET. The ROM
 *     loads &REG_SIODATA32 and &REG_SIOCNT from two separate pool words and
 *     keeps the SIOCNT one in r0 for the later `strh r3, [r0, #2]`. This draft
 *     reassigns one `vu32 *` and gcc's cse derives 0x4000128 from 0x4000120 with
 *     `adds r2, #8`. Per docs/elevation.md a runtime DERIVE proves ONE symbol
 *     plus an offset and two pooled words prove TWO -- so the source wants two
 *     independent pointers (or the REG_SIOCNT macro) in a shape cse will not
 *     fold. This is the first divergence, at index 1, and everything after it is
 *     displaced.
 *
 *  2. THE FRAME ADDRESS IS A PSEUDO FROM THE FIRST STORE. The ROM does
 *     `mov lr, sp / mov r2, lr / str r3, [r2] / str r4, [r2, #4]` -- the two
 *     word stores into the 8-byte local go through the SAME register the later
 *     `ldrh r3, [r2, r5]` indexed read needs. This draft writes `buf[0] = ...`
 *     and gets sp-relative immediates (`str r3, [sp, #0]`). Routing every access
 *     through one `u32 *` local is the untried fix; it is the same lever that
 *     took Func_800615c from 93 to 86 (see src/non_matching/rom_c0/800615c.c,
 *     "THE 8-BYTE LOCAL IS A BYTE ARRAY").
 *
 * WHAT THE DRAFT ALREADY GETS RIGHT: the `(siocnt << 25) >> 31` bit extraction
 * (two shifts, the spelling docs/elevation.md records for this bank -- see
 * src/non_matching/rom_c0/8006088.c), the -1 / >= 0 three-way on f14, the
 * `i * 4` offset held in r12 and added to 0x18 / 0x30 / 0x40 for the three
 * parallel member arrays, and the `if (x <= 14) x++` clamp pair.
 *
 * STRUCT LAYOUT, read off the offsets and consistent with the three landed
 * neighbours: f0 u8; f4[2] u8 flags; f9 u8; f14 int; f18[2] int; f28/f2c u16*
 * (swapped as a pair); f30[4] / f40[4] u16* (swapped element-wise). f28/f2c and
 * f30/f40 are the same pointers Func_8005d10 plants and Func_800615c swaps.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/8006240.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_c.s --func Func_8006240
 */
#include "gba/types.h"
#include "gba/io.h"

struct Snd {
    /* 0x00 */ unsigned char f0;
    /* 0x01 */ unsigned char f1[3];
    /* 0x04 */ unsigned char f4[5];
    /* 0x09 */ unsigned char f9;
    /* 0x0a */ unsigned char fa[10];
    /* 0x14 */ int f14;
    /* 0x18 */ int f18[2];
    /* 0x20 */ unsigned char f20[8];
    /* 0x28 */ unsigned short *f28;
    /* 0x2c */ unsigned short *f2c;
    /* 0x30 */ unsigned short *f30[4];
    /* 0x40 */ unsigned short *f40[4];
};

extern struct Snd ewram_2002240;

void Func_8006240(void)
{
    u32 buf[2];
    vu32 *sio;
    unsigned short *q;
    unsigned short *r;
    unsigned short w;
    int n;
    int i;

    sio = (vu32 *)REG_ADDR_SIODATA32;
    buf[0] = sio[0];
    buf[1] = sio[1];
    sio = (vu32 *)REG_ADDR_SIOCNT;
    ewram_2002240.f9 = (*sio << 25) >> 31;
    if (ewram_2002240.f14 == -1) {
        q = ewram_2002240.f2c;
        *(vu16 *)(REG_ADDR_SIOCNT + 2) = 0xfefe;
        ewram_2002240.f2c = ewram_2002240.f28;
        ewram_2002240.f28 = q;
    } else if (ewram_2002240.f14 >= 0) {
        *(vu16 *)(REG_ADDR_SIOCNT + 2) = ewram_2002240.f2c[ewram_2002240.f14];
    }
    if (ewram_2002240.f14 <= 14)
        ewram_2002240.f14++;
    for (i = 0; i < 2; i++) {
        if (((unsigned short *)buf)[i] == 0xfefe && ewram_2002240.f18[i] > 13) {
            ewram_2002240.f18[i] = -1;
        } else {
            n = ewram_2002240.f18[i];
            q = ewram_2002240.f30[i];
            w = ((unsigned short *)buf)[i];
            q[n] = w;
            if (n == 13) {
                r = ewram_2002240.f40[i];
                ewram_2002240.f40[i] = q;
                ewram_2002240.f30[i] = r;
                ewram_2002240.f4[i] |= 1;
            }
        }
        if (ewram_2002240.f9 != 0)
            ewram_2002240.f4[i] |= 2;
        if (ewram_2002240.f18[i] <= 14)
            ewram_2002240.f18[i]++;
    }
    if (ewram_2002240.f0 == 8) {
        REG_TM3CNT_H = 0;
        REG_SIOCNT |= 0x80;
        REG_TM3CNT_H = 0xc0;
    }
}
