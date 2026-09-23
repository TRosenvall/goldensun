/* StartTitleScreen -- NON-MATCHING, 363 encodings of 493, size 1144 against the ROM's
 * 1152 (-8), 489 instructions against 493.  438 instructions in the reference.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_f2000/StartTitleScreen.c \
 *     asm/rom_f2000/rom_f2028_a.s --func StartTitleScreen
 * THREE functions in the reference (Func_80f2028, LoadGS1TitleGFX, StartTitleScreen);
 * a text split is required.  No data sections, but it references .Lf38bc and .Lf39b1,
 * which are .global-exported from asm/rom_f2000/rom_f2028_c_c_c_c.s -- two
 * `extern unsigned char Data_...[]` declarations and NO data split.
 *
 * **MEASURE THIS ONE BY RELOCATION OFFSETS, NOT BY THE POSITIONAL COUNT** -- the
 * instruction counts disagree (489 against 493), so by the fourth angle of the
 * measurement rule the ordering is unreliable.  On that basis:
 * RELOCATIONS 0-9 ARE BYTE-EXACT AND 11-17 AND 19 ARE BYTE-EXACT -- the prologue, the
 * DMA-queue inline, StartTask, and the whole first animation section through __modsi3 at
 * 0x1aa all land on the ROM's addresses.
 *
 * FOUR RESIDUES:
 *   1. +2 insns at reloc 10 (0xdc against 0xe0): gcc makes THREE low-register copies of
 *      `t` (r8) in the frame-advance block where the ROM makes two.  A named
 *      `unsigned char *w = t;` alias was INERT -- gcc coalesces it.
 *   2. -2 insns in the `m*8 <= 0x18` DMA branch (reloc 18 drifts -4).
 *   3. -2 insns in the GetFile / DMA / SET_PALETTE region (reloc 24 drifts -8), plus the
 *      pool-word placement for gBuffer and .Lf38bc (relocs 27-29, 39).
 *   4. Index 9 is `mov r0,#0` against `mov r2,#0` -- the cse const-0 class documented in
 *      src/non_matching/rom_15000/801c49c.c, which also records WHY the asm barrier
 *      cannot reach it (cse.c:5745 flushes only on ASM_OPERANDS, and a no-operand
 *      `asm volatile("")` is not one).
 *
 * ================================================================
 * TWO STRUCTURAL FINDINGS, both worth more than the count
 * ================================================================
 *
 * LOOPS IN THIS BANK ARE A MIX, AND THE TELL IS "IS A CONSTANT REBUILT INSIDE THE BODY".
 * This function has five.  The main frame loop is a real `for(;;)` -- loop.c hoists
 * `&iwram_3001ad0`, the constant 1 and `&h` into a SEVEN-INSTRUCTION PREHEADER.  The
 * BG-fill, tile-fill and sprite-slot loops each REBUILD a constant in the body
 * (`mov r0,#0x80 / lsl r0,#9`, `ldr r3,=0x40004000`, `mov r1,#0x80`) and are therefore
 * GOTO LOOPS WITH NO LOOP NOTES AND NO LICM.  Writing them as do/while hoists those
 * constants and is wrong.  That is the recorded goto-loop signature firing three times
 * in one function.
 *
 * A BLOCK SITTING BETWEEN A LOOP'S `b test` AND ITS BODY MEANS THE LOOP LIVES *AFTER*
 * THE ENCLOSING LOOP, ENTERED BY A GOTO.  In the ROM the `WaitFrames(1); b loop_top`
 * continue-block sits physically BETWEEN the tile loop's `b .Lf29ea` and its body
 * `.Lf29c4`.  gcc-2.96 has no block-reordering pass, so that is SOURCE ORDER.
 * Reproducing it needs the tile loop AND EVERYTHING AFTER IT lifted out of the `else if`
 * body, with a `goto` from inside the loop; jump.c then collapses `goto LT` -> `goto LTT`
 * and deletes the trampoline.  THAT ONE RESTRUCTURE TOOK 407 -> 363 AND MADE RELOCATIONS
 * 11-17 AND 19 BYTE-EXACT.
 *
 * `__asm__ volatile("")` AS THE FIRST STATEMENT is the right directional barrier for a
 * prologue-order residue -- it stopped sched2 hoisting `mov rN,#0` above gcc's own
 * parameter spill `str r0,[sp,#0xc]` and fixed encoding index 8.  The
 * `do { } while (0)` form was identical here.
 *
 * `cmp #N / bne` REALLY IS an if/else-if chain here and not a switch: the
 * 0x119 / 0x121 / 0x118 chain reproduces from a plain `else if` chain.  No jump tables in
 * this function.
 *
 * A tryc BLIND SPOT CONFIRMED INDEPENDENTLY HERE, on SIX sites (0x13b, 0x681, 0x1440, 0,
 * 0x1540, 0x1ff): `ldrh rX, .L<pool>` in gcc's output is THE SAME ENCODING as the ROM's
 * `ldr rX, .L<pool>`, because *thumb_movhi_insn alt 1 prints `ldrh` and GAS assembles a
 * two-byte pc-relative WORD load.  Six correct bare literals were nearly converted to
 * `int` carriers on that false signal.  Corollary recorded with it: `ldr rX, =0` in a
 * hand-written `.s` feeding a `strh` is a HImode pool load, not an impossible SImode
 * pooled zero -- and it is what produces this bank's mid-function pool dumps at
 * pool_range 64.
 *
 * REUSABLE DONORS FOUND: include/dma.h's DMA3_SET reproduces all four
 * `stmia r3!, {r0,r1,r2} / sub r3,#0xc` sites unaided;
 * src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_b.c's LOCK_IME plus queue inline reproduces
 * the gDMATaskCount enqueue; `SET_PALETTE(0, 0xa0 << 19)` reproduces the
 * `strh r1,[r3]` address-into-itself idiom.
 *
 * No per-file Makefile flag override applies to this stem.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "palette.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern volatile int gKeyPress;
extern unsigned char iwram_3001f58;
extern unsigned char iwram_3001d18;
extern unsigned short iwram_3001ad0[];
extern unsigned char gBuffer[];
extern unsigned char ewram_2012580[];
extern unsigned char ewram_20199c0[];
extern unsigned char Data_f38bc[];
extern unsigned char Data_f39b1[];
extern int _FILE_16;

extern unsigned char *galloc_iwram(int tag, int size);
extern unsigned char *galloc_ewram(int tag, int size);
extern void gfree(int tag);
extern void ClearVRAM(void);
extern void ClearSprites(void);
extern void ClearTasks(void);
extern void WaitFrames(int n);
extern void LoadGS1TitleGFX(void);
extern void Func_80f377c(void);
extern void Func_80f3824(int a, int b);
extern void Func_80f3858(int a);
extern void Func_80f2028(void);
extern int StartTask(void *fn, int pri);
extern void StopTask(void *fn);
extern void Func_8003bf8(int a);
extern void Func_8003c3c(int a);
extern void Func_8003ce0(void);
extern void Func_8003dec(void *p, int n);
extern void Func_800479c(void);
extern unsigned char *GetFile(int id);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueueDma(struct DmaQueue *queue)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = 0xf740;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

int StartTitleScreen(int mode)
{
    unsigned char *t;
    unsigned char *q;
    unsigned char *u;
    unsigned char *v;
    unsigned char *file;
    unsigned char *gfx;
    unsigned short *p;
    unsigned short h;
    unsigned short *hp;
    int ret;
    unsigned char saved;
    int f;
    int g;
    int c;
    int m;
    int n;
    short tile;
    unsigned int i;
    unsigned int j;
    unsigned int lim;
    int x;

    __asm__ volatile ("");
    ret = 0;
    saved = iwram_3001f58;
    t = galloc_iwram(0x2b, 0xe0);
    ClearVRAM();
    ClearSprites();
    WaitFrames(1);
    ClearTasks();
    iwram_3001d18 = ret;
    iwram_3001f58 = ret;
    LoadGS1TitleGFX();
    Func_80f377c();
    Func_80f3824(2, 0);
    QueueDma(&gDMATaskCount);
    Func_80f3858(0x3c);
    StartTask(Func_80f2028, 0x90 << 3);
    hp = &h;
    for (;;) {
        f = *(int *)(t + 8);
        if ((unsigned int)(f - 0x15) <= 0xd9 && (gKeyPress & 9) != 0) {
            *(int *)(t + 0x10) = 1;
            *(int *)(t + 8) = 0xef;
            f = 0xef;
        }
        g = f + 1;
        *(int *)(t + 8) = g;
        if (g <= 0x8b * 2) {
            c = *(int *)(t + 0xc);
            if (c % 3 == 0) {
                iwram_3001ad0[3] = iwram_3001ad0[3] - 1;
                if ((iwram_3001ad0[3] & 7) == 0) {
                    n = *(int *)t;
                    DMA3_SET(ewram_2012580 - n * 15 * 64,
                             (void *)(0x6004b00 - n * 15 * 64), 0x800001e0);
                    *(int *)t = *(int *)t + 1;
                    c = *(int *)(t + 0xc);
                }
            }
            if ((c & 1) == 0) {
                iwram_3001ad0[5] = iwram_3001ad0[5] - 1;
                if ((iwram_3001ad0[5] & 7) == 0) {
                    m = *(int *)(t + 4);
                    if (m * 8 <= 0x18) {
                        DMA3_SET(ewram_20199c0 - m * 15 * 128,
                                 (void *)(0x600e4c0 - m * 15 * 128), 0x800003c0);
                    } else {
                        *hp = 0;
                        DMA3_SET(hp,
                                 (void *)(((0xa0 - m * 8) % 0xa0) * 15 * 16 + 0x6004ec0),
                                 0x810003c0);
                        p = (unsigned short *)0x600f6c0;
                        j = 0;
                    LA:
                        i = 0;
                    LB:
                        i++;
                        *p++ = 0x13b;
                        if (i <= 0x1d)
                            goto LB;
                        *p++ = 0x13b;
                        j++;
                        *p++ = 0x13b;
                        if (j <= 4)
                            goto LA;
                    }
                    *(int *)(t + 4) = *(int *)(t + 4) + 1;
                }
            }
            if (*(int *)(t + 8) == 0xef)
                *(int *)(t + 0x10) = 1;
        } else if (g == 0x119) {
            *(int *)(t + 0x10) = 2;
        } else if (g == 0x121) {
            *(int *)(t + 0x10) = 0;
        } else if (g == 0x8c * 2) {
            Func_8003bf8(1);
            StopTask(Func_80f2028);
            iwram_3001d18 = 1;
            WaitFrames(1);
            REG_BG2CNT = 0x681;
            REG_DISPCNT = 0x1440;
            iwram_3001ad0[5] = 0;
            file = GetFile((int)&_FILE_16);
            DMA3_SET(file, (void *)(0xa0 << 19), 0x84000078);
            SET_PALETTE(0, 0xa0 << 19);
            DecompressLZ1(file + 0x80 * 4, gBuffer);
            DMA3_SET(gBuffer, (void *)0x6004000, 0x80004b00);
            tile = 0x80 << 1;
            p = (unsigned short *)0x6003000;
            j = 0;
            goto LT;
        }
        WaitFrames(1);
    }
LT:
    goto LTT;
LTB:
    i = 0;
LTI:
    i++;
    *p++ = tile;
    tile = tile + 1;
    if (i <= 0x1d)
        goto LTI;
    *p++ = 0x1ff;
    j++;
    *p++ = 0x1ff;
LTT:
    if (j <= 0x13)
        goto LTB;
    Func_800479c();
    ClearVRAM();
    if (mode != 0) {
        q = galloc_ewram(0xe, 0x80 << 3);
        DecompressLZ1(Data_f38bc, q);
        u = t + 0x80;
        i = 0;
    LF:
        gfx = (unsigned char *)UploadSpriteGFX(AllocSpriteSlot(), 0x80,
                                               q + ((i * 0x100) >> 1));
        v = u;
        *(int *)v = 0;
        v += 4;
        *(int *)v = 0x40004000;
        v += 4;
        *(int *)v = (int)gfx;
        i++;
        u += 0xc;
        if (i <= 4)
            goto LF;
        gfree(0xe);
    }
    Func_8003c3c(0x1e);
    Func_8003ce0();
    REG_DISPCNT = 0x1540;
    lim = 0x96 * 2;
    if (mode != 0)
        lim = 0xe1 << 4;
    for (i = 0; i < lim; i++) {
        if (mode != 0) {
            u = t + 0x80;
            j = 0;
            x = 0x50;
            do {
                *(unsigned short *)(u + 6) =
                    (*(unsigned short *)(u + 6) & 0xfffffe00) | (x & 0x1ff);
                u[4] = 0x7c;
                Func_8003dec(u, 0);
                j++;
                x += 0x20;
                u += 0xc;
            } while (j <= 2);
            n = Data_f39b1[i % 0x3c];
            REG_BLDCNT = 0x2f50;
            REG_BLDALPHA = ((0x10 - n) << 8) + n;
        }
        if ((gKeyPress & 9) != 0) {
            ret = 1;
            goto L9;
        }
        WaitFrames(1);
    }
L9:
    iwram_3001f58 = saved;
    gfree(0x2b);
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    WaitFrames(1);
    return ret;
}
