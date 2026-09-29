/* LoadGS1TitleGFX (0x080f24a0) -- NON-MATCHING, 196 encodings of 251 differ.
 * 209 ROM instructions.  Frontier target: neither this project nor the parallel
 * decompilation had attempted it (docs/parallel-coverage.tsv).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_f2000/80f24a0_LoadGS1TitleGFX.c \
 *       asm/rom_f2000/rom_f2028_a.s --func LoadGS1TitleGFX
 *
 * 196 IS NOT A DISTANCE, and badly so.  Size 604 against 588, count 257 against
 * 251 -- and this reference keeps THREE literal pools INSIDE the function, so
 * every `ldr [pc,#N]` after the first shift is counted as differing.  The
 * navigation figure is `tryc --align`: 107 of 220, with the first 24 exact.
 *
 * BLOCKER, named by the pass, and the dump says it outright: `.08.loop`,
 * loop.c's strength_reduce --
 *   Insn 403: giv reg 84 src reg 37 benefit 4 lifetime 1 replaceable
 *             mult 65536 add 65536
 * i.e. it makes `(t << 16) + 0x10000` a general induction variable carried
 * across the back edge, where the ROM recomputes it in the loop.  That single
 * giv IS the whole 107, repeated across all four tilemap loops.  The same dump
 * shows loop.c DOES reject other givs on cost, so the ROM priced this one as
 * unprofitable -- meaning the lever is register pressure, not the source shape.
 * Three spellings measured, all identical.
 *
 * The first 24 were closed by a volatile-qualified read pinning the `state` load
 * to the top PLUS the r7 pin; neither is sufficient alone.  The push list was
 * the diagnosis: the first candidate saved one extra high register, which was
 * gcc pushing `state` to r10 and spending the freed r7 on a hoisted constant.
 *
 * SHIMS: 1 register pin (`state` -> r7).  A landing needs a fakematch.txt row.
 *
 * SPLIT: anchored grep gives THREE functions in asm/rom_f2000/rom_f2028_a.s --
 * Func_80f2028, LoadGS1TitleGFX and StartTitleScreen -- so a three-way split.
 * datacheck.py reports nothing: no data section, no exports.  Worth knowing
 * before anyone starts: the other two are ALREADY PARKED
 * (src/non_matching/rom_f2000/StartTitleScreen.c and the Func_80f2028 park), so
 * the split should be done once for all three rather than three times.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "file_table.h"

extern unsigned char iwram_3001efc[];
extern short iwram_3001ad0[];
extern unsigned char ewram_2012940[];
extern unsigned char ewram_201a140[];
extern unsigned char gBuffer[];

extern void DecompressLZ1(void *src, void *dst);

void LoadGS1TitleGFX(void)
{
    register s32 *state __asm__("r7");
    unsigned char *file;
    unsigned char *buf;
    short *h;
    u16 *p;
    short t;
    short pad;
    s32 off;
    s32 row;
    s32 n;

    state = *(s32 *volatile *)iwram_3001efc;
    REG_DISPCNT = 0;
    file = (unsigned char *)GetFile(FILE_15);
    DMA3_SET(file, (void *)0x5000200, 0x84000080);
    *(vu16 *)0x5000200 = 0;
    off = 0x80 << 2;
    buf = gBuffer;
    file += off;
    DecompressLZ1(file, buf);
    DMA3_SET(buf, (void *)0x6010000, 0x80000f00);
    file = (unsigned char *)GetFile(FILE_17);
    DMA3_SET(file, (void *)(0xa0 << 19), 0x84000080);
    *(vu16 *)(0xa0 << 19) = 0;
    file += off;
    DecompressLZ1(file, buf);
    DMA3_SET(ewram_2012940, (void *)(0xc0 << 19), 0x80002760);
    DMA3_SET(ewram_201a140, (void *)0x6004ec0, 0x80004ec0);

    pad = 0x1ff;
    p = (u16 *)0x600f000;
    t = 0x267;
    row = 0;
    do {
        n = 0x1d;
        do {
            *p = t;
            t++;
            n--;
            p++;
        } while (n >= 0);
        *p = pad;
        row++;
        p++;
        *p = pad;
        p++;
    } while (row <= 0xa);
    t = 0x13b;
    row = 0xb;
    do {
        n = 0x1d;
        do {
            *p = t;
            t++;
            n--;
            p++;
        } while (n >= 0);
        *p = pad;
        row++;
        p++;
        *p = pad;
        p++;
    } while (row <= 0x1f);
    t = 0x96 << 1;
    p = (u16 *)0x600f800;
    row = 0;
    do {
        n = 0x1d;
        do {
            *p = t;
            t++;
            n--;
            p++;
        } while (n >= 0);
        *p = pad;
        row++;
        p++;
        *p = pad;
        p++;
    } while (row <= 0xa);
    t = 0;
    row = 0xb;
    do {
        n = 0x1d;
        do {
            *p = t;
            t++;
            n--;
            p++;
        } while (n >= 0);
        *p = pad;
        row++;
        p++;
        *p = pad;
        p++;
    } while (row <= 0x1f);

    REG_BG1CNT = 0x1f43;
    REG_BG2CNT = 0x1e81;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x9f;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x9f;
    REG_WININ = 0x1616;

    h = iwram_3001ad0;
    n = 3;
    do {
        h[1] = 0;
        h[0] = 0;
        h += 2;
        n--;
    } while (n >= 0);
    h = iwram_3001ad0;
    h[3] = 0x60;
    h[5] = 0x60;
    state[0] = 0;
    state[1] = 0;
    state[2] = 0;
    state[3] = 0;
    state[4] = 0;
    state[5] = 0;
    DMA3_SET(h, (void *)&REG_BG0HOFS, 0x84000004);
    REG_BLDCNT = 0x3fbf;
    REG_BLDALPHA = 0x1010;
    REG_BLDCNT = 0x3f44;
}
