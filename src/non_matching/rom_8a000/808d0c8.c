/* Debug_PaletteEditor  --  asm/rom_8a000/rom_8ba38_a_c_a_c.s  (0x0808d0c8)
 *
 * NON-MATCHING: 329 encodings of 337 differ (objcmp).
 * Working distance: 380 instructions in disagreeing regions of 352 (tryc --align).
 * NOT a true distance, and the align figure is larger than the ROM's own length, which is what
 * a candidate looks like when whole regions are still misaligned rather than mis-allocated.
 * Length 334 against the ROM's 352. FIRST CANDIDATE -- opened this batch, not refined.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808d0c8.c asm/rom_8a000/rom_8ba38_a_c_a_c.s --whole
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808d0c8.c --ref asm/rom_8a000/rom_8ba38_a_c_a_c.s --align
 *
 * Whole-file conversion: one function, no data. No pins, no flags. ZERO shims.
 * The `.s` comment block names it UpdateFollowerPositions and says "~180-instruction body";
 * both are wrong for this file -- it is 321 instructions and it is a palette editor.
 *
 * WHAT THE FUNCTION IS (read out of the ROM, and this part is settled)
 *   No arguments, no return. A debug palette editor with three cursors kept in high registers:
 *   r9 = row 0..0xd, r10 = channel 1..3, r8 = column 1..0xf. It decompresses a tile image to
 *   0x6001a00, then loops: redraw the BG map rows at 0x600205a..0x600211c from the palette at
 *   0x5000002 + row*32, WaitFrames(1), then poll gKeyRepeat bit by bit --
 *     0x40 / 0x80  channel down / up, wrapping 1..3
 *     0x20 / 0x10  column  down / up, wrapping 1..0xf
 *     0x200 / 0x100 row    down / up, wrapping 0..0xd, each followed by a redraw
 *     1    / 2     A / B: increment or decrement the selected 5-bit channel of the halfword at
 *                  0x5000000 + ((row << 4) + col) * 2, clamped at 0x1e / 0, then redraw
 *     8            hold: flash the selected entry white / original / black / original on a
 *                  40-frame cycle while gKeyHeld & 8, then restore it
 *     4            exit via Func_800479c() and ClearVRAM()
 *   Two derived values are carried across the redraw: row << 12 (in the frame at sp+8) and
 *   row << 5 (in r11). The ROM has FOUR entry points into the redraw block
 *   (.L8d0f2 / .L8d108 / .L8d10e / .L8d112) which are jump2 cross-jumping the two assignments
 *   shared by the two row-changing branches -- with row == 0 the `row << 12` folds to a stored
 *   zero, which is why one entry point skips straight to `mov r11, r1`.
 *
 * THE ONE FINDING HERE THAT MATTERS MORE THAN THE NUMBER, AND IT IS THE OPPOSITE OF THIS
 * BATCH'S ScreenTransitionIn LEVER:
 *   The ROM's `ldr r3, .L8d144  @ 0xf052` / `.L8d148 @ 0xf047` / `.L8d14c @ 0xf042` and, more
 *   surprisingly, `ldr r5, .L8d150 @ 0x1f` and `ldr r4, .L8d154 @ 0xf0e0`, are gcc's own
 *   `ldrh rD, <label>` over a pool word -- the batch-292 correction, same encoding as `ldr`.
 *   They are what a HImode CONST_INT costs, so here the LITERALS ARE CORRECT and an int local
 *   would be wrong. 0x1f being pooled is the tell that the masking is narrowed to HImode by
 *   combine, i.e. the stores really are written as `*(u16 *)X = <literal-bearing expression>`.
 *   ScreenTransitionIn in this same brief needs the exact opposite (int locals) because ITS
 *   ROM has `mov r3,#0x3f / strh`. So "HImode literal store" is not a rule in either
 *   direction: read whether the ROM materialises the value with a `mov` or a pool load, per
 *   site, and match that. Two functions in one bank in one batch want opposite answers.
 *
 * LEVER THAT PAID: A RUNNING POINTER IN THE DRAW LOOP, 414 -> 380.
 *   The ROM walks the four map rows with ONE register: `mov r2,r1 / add r2,#0x40 / strh /
 *   add r2,#0x40 / strh / add r2,#0x40 / strh`. Writing the three stores as
 *   `*(u16 *)((char *)p + 0x40/0x80/0xc0)` instead makes gcc compute three independent
 *   addresses and it runs out of low registers -- the emitted loop uses BOTH `ip` and `lr` as
 *   address temporaries. A single `d` advanced by 0x40 three times removes that.
 *
 * THE FOUR THINGS TO FIX NEXT, IN ORDER OF SIZE
 *  1. THE DRAW LOOP IS REVERSED IN OURS AND NOT IN THE ROM. The ROM counts UP in r12 with an
 *     UNSIGNED bound (`add r12,r3 / mov r2,r12 / cmp r2,#0xf / bls`); ours counts down from 14.
 *     The bound is a CONST_INT so check_dbra_loop's vanilla path is open, and what normally
 *     shuts it is loop.c:7896's `giv_count == 0` test -- this loop has three givs (the tile
 *     number, the palette pointer and the map pointer), so the ROM's non-reversal is expected
 *     and OURS is the anomaly. Read `.08.loop` and find which of the three givs we lost; the
 *     running-pointer change above already recovered one and was worth 34.
 *  2. sp IS 4 BYTES IN OURS AND 0xc IN THE ROM. The ROM's three slots are `row << 12` plus a
 *     spill of BOTH the gKeyRepeat pool address (r1) and the mask 0x1f (r4) across every
 *     WaitFrames call (`str r1,[sp,#4] / str r4,[sp]` ... reload after). We rematerialise
 *     both. This is the calls.c:855 argument-precompute lever from the other direction: the
 *     ROM pays to keep two loop-invariant values live across a call, so they are source
 *     variables, and `0x1f` in particular wants naming ONCE and using at all six masking
 *     sites (the brief's "name it at ALL sites, or not at all").
 *  3. THE FOUR REDRAW ENTRY POINTS (22 ROM instructions we emit as 5). We collapse them
 *     because gcc constant-propagates `row = 0` into the initial `rowhi = 0; rowlo = 0;` and
 *     then recomputes the shifts at the redraw block instead of carrying them. The ROM's shape
 *     says both derived values are live-in to the redraw from four predecessors. Suspect the
 *     same gcse-cprop-versus-cse split as Func_808e9c0's 0x80000 in this brief; check whether
 *     the redraw block is entered from enough predecessors to block cprop once the loop shape
 *     is right, before reaching for any construct.
 *  4. The dead `ldr r3,=iwram_3001e40 / ldr r3,[r3]` before the last WaitFrames is a read whose
 *     value is discarded -- we model it as `i = iwram_3001e40;` with iwram_3001e40 volatile,
 *     which is the shape the corpus already uses for this symbol (three landed files declare it
 *     volatile). Not yet isolated; it is 2 instructions and should be measured last.
 */
#include "gba/types.h"

extern const void *L9e4ce __asm__(".L9e4ce");
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyHeld;
extern volatile unsigned int iwram_3001e40;

extern void DecompressLZ16(const void *src, void *dst);
extern void WaitFrames(int n);
extern void Func_800479c(void);
extern void ClearVRAM(void);

void Debug_PaletteEditor(void);

void Debug_PaletteEditor(void)
{
    u16 *p;
    u16 *d;
    u16 *pal;
    u16 *sel;
    int row;
    int ch;
    int col;
    int rowhi;
    int rowlo;
    int t;
    int i;
    int c;
    int r;
    int g;
    int b;
    int f;
    unsigned int k;

    row = 0;
    ch = 1;
    col = 1;
    DecompressLZ16(&L9e4ce, (void *)0x6001a00);
    rowhi = 0;
    rowlo = 0;
redraw:
    *(u16 *)0x600205a = row + 0xfffff0e0;
    *(u16 *)0x600209a = 0xf052;
    *(u16 *)0x60020da = 0xf047;
    *(u16 *)0x600211a = 0xf042;
    p = (u16 *)0x600205c;
    t = rowhi + 0xd1;
    pal = (u16 *)(0x5000002 + rowlo);
    i = 1;
    do {
        *p = t;
        c = *pal;
        d = (u16 *)((char *)p + 0x40);
        *d = (c & 0x1f) + 0xf0e0;
        d = (u16 *)((char *)d + 0x40);
        *d = ((c >> 5) & 0x1f) + 0xf0e0;
        d = (u16 *)((char *)d + 0x40);
        *d = ((c >> 10) & 0x1f) + 0xf0e0;
        i++;
        t++;
        pal++;
        p++;
    } while (i <= 0xf);
    WaitFrames(1);
keys:
    if (gKeyRepeat & 0x40) {
        ch--;
        if (ch <= 0)
            ch = 3;
    }
    if (gKeyRepeat & 0x80) {
        ch++;
        if (ch > 3)
            ch = 1;
    }
    if (gKeyRepeat & 0x20) {
        col--;
        if (col <= 0)
            col = 0xf;
    }
    if (gKeyRepeat & 0x10) {
        col++;
        if (col > 0xf)
            col = 1;
    }
    if (gKeyRepeat & 0x200) {
        row--;
        if (row < 0)
            row = 0xd;
        rowhi = row << 12;
        rowlo = row << 5;
        goto redraw;
    }
    if (gKeyRepeat & 0x100) {
        row++;
        if (row > 0xd)
            row = 0;
        rowhi = row << 12;
        rowlo = row << 5;
        goto redraw;
    }
    if (gKeyRepeat & 1) {
        sel = (u16 *)(((row << 4) + col) * 2 + (0xa0 << 19));
        c = *sel;
        r = c & 0x1f;
        g = (c >> 5) & 0x1f;
        b = (c >> 10) & 0x1f;
        if (ch == 1 && r <= 0x1e)
            r++;
        if (ch == 2 && g <= 0x1e)
            g++;
        if (ch == 3 && b <= 0x1e)
            b++;
        *sel = (b << 10) | (g << 5) | r;
        goto redraw;
    }
    if (gKeyRepeat & 2) {
        sel = (u16 *)(((row << 4) + col) * 2 + (0xa0 << 19));
        c = *sel;
        r = c & 0x1f;
        g = (c >> 5) & 0x1f;
        b = (c >> 10) & 0x1f;
        if (ch == 1 && r != 0)
            r--;
        if (ch == 2 && g != 0)
            g--;
        if (ch == 3 && b != 0)
            b--;
        *sel = (b << 10) | (g << 5) | r;
        goto redraw;
    }
    if (gKeyRepeat & 8) {
        sel = (u16 *)(((row << 4) + col) * 2 + (0xa0 << 19));
        c = *sel;
        f = 0;
        goto flashwait;
flash:
        if (f == 0)
            *sel = 0x7fff;
        if (f == 0xa)
            *sel = c;
        if (f == 0x14)
            *sel = 0;
        if (f == 0x1e)
            *sel = c;
        f++;
        if (f > 0x27)
            f = 0;
flashwait:
        WaitFrames(1);
        if (gKeyHeld & 8)
            goto flash;
        *sel = c;
    }
    if ((gKeyRepeat & 4) == 0) {
        i = iwram_3001e40;
        WaitFrames(1);
        goto keys;
    }
    Func_800479c();
    ClearVRAM();
}
