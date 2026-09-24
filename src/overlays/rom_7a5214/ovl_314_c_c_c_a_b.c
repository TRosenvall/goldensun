/* OvlFunc_918_2009244 -- cut from the HEAD of
 * asm/overlays/rom_7a5214/ovl_314_c_c_c_a.s, which holds TWO functions:
 * OvlFunc_918_2009244 (this one) and OvlFunc_918_2009424, which stays behind.
 * tools/datacheck.py reports no `.section .data` in that file, so the split is a
 * pure text cut -- but it IS a split and the remaining .s has to keep the second
 * function.
 *
 * The 63-entry palette ramp: every 32nd frame, darken each of the three colour
 * deltas from the .L1f00 table by a fraction that depends on how far up the ramp
 * we are, add them to the source palette, clamp each channel to 0..0x1f and
 * write the result to BG palette RAM; then advance the table index by a random
 * multiple of three, wrapping on the 0x63 sentinel.
 *
 * THREE LEVERS, and the first is the one to remember.
 *
 * 1. `+=` RATHER THAN `= ... +`, AND IT CHOSE WHICH VALUE GOT SPILLED.
 *    Written `r = (px & 0x1f) + a;` the three channel adds are three-operand
 *    (`add r6, r3, r6`) and land in the delta's own register, because the delta
 *    is dead afterwards.  The ROM has the two-operand `add r0, r4` -- destination
 *    is the MASKED CHANNEL's register -- which is what `r = px & 0x1f;` followed
 *    by a separate `r += a;` gives.  205 differing against 27 for that one
 *    change, and the interesting part is WHY it is worth 178: four values are
 *    live across the `__divsi3` calls (the three deltas plus the shift temp) and
 *    only three callee-saved registers are free, so one delta has to go to the
 *    stack slot.  With the fused form gcc spills the BLUE delta; the ROM spills
 *    the RED one.  Permuting the declarations, permuting the three loads, naming
 *    the per-chain temp and splitting the subtraction were ALL inert on that
 *    choice -- the only thing that moved it was the shape of the add at the far
 *    end of the loop body.  So when a register-allocation residue looks
 *    structural, check whether a LATER use of the value is what is deciding it.
 *
 * 2. BOTH `iwram_3001ebc` WORDS ARE READ BEFORE THE FIRST GUARD, and the one the
 *    guard uses needs a NAME.  Reading the palette source inside/after the
 *    guards is 214 differing; reading it before them but leaving the guard's own
 *    `iwram_3001ebc[0]` inline is 27; naming it `p` is 12.  The array form
 *    `extern unsigned char *iwram_3001ebc[];` is the one this tree uses when two
 *    different words of it are wanted.
 *
 * 3. `i++` GOES LAST IN THE LOOP BODY.  Before the store it is emitted at the top
 *    of the tail block; the ROM has `add r10, r1` between the last `orr` and the
 *    `strh`.  `i++` after `src++` (or `while (++i <= 0x3e)`, or `*dst++ =`) all
 *    give the ROM.  `px` must be `unsigned` -- as `int` the three masks cost two
 *    encodings.
 *
 * The three fractions read off the shifts: gcc does signed `/2` with
 * `lsr #31 / add / asr #1` and signed `/4` with `cmp / bge / add #3 / asr #2`,
 * so the arms are `v -= v/2 + v/3`, `v -= v/3 + v/4`, `v -= v/4 + v/5`.
 *
 * VERIFIES: 480 bytes, 225 encodings and 14 relocations identical.  objcmp's nine
 * `~~` notes are its own alias resolution for `__divsi3 = _divsi3_RAM;`, which
 * this overlay's ld already carries at line 91.
 */
extern unsigned char *iwram_3001ebc[];
extern unsigned int iwram_3001e40;
extern int L2db8 __asm__(".L2db8");
extern int L1f00[] __asm__(".L1f00");
extern unsigned int __Random(void);

void OvlFunc_918_2009244(void)
{
    unsigned short *src;
    unsigned short *dst;
    unsigned int i;
    int a, b, c;
    int k;
    int r, g, bl;
    unsigned int px;
    unsigned char *p;

    p = iwram_3001ebc[0];
    src = (unsigned short *)iwram_3001ebc[5];
    if (*(short *)(p + 0x17e) != 0)
        return;
    if ((iwram_3001e40 & 0x1f) != 0)
        return;
    src += 0x10;
    dst = (unsigned short *)0x5000020;
    i = 0;
    do {
        k = L2db8;
        a = L1f00[k];
        b = L1f00[k + 1];
        c = L1f00[k + 2];
        if (i > 0x2f) {
            a -= a / 2 + a / 3;
            b -= b / 2 + b / 3;
            c -= c / 2 + c / 3;
        } else if (i > 0x1f) {
            a -= a / 3 + a / 4;
            b -= b / 3 + b / 4;
            c -= c / 3 + c / 4;
        } else if (i > 0xf) {
            a -= a / 4 + a / 5;
            b -= b / 4 + b / 5;
            c -= c / 4 + c / 5;
        }
        px = *src;
        r = px & 0x1f;
        g = (px >> 5) & 0x1f;
        bl = (px >> 10) & 0x1f;
        r += a;
        g += b;
        bl += c;
        if (r > 0x1f)
            r = 0x1f;
        if (g > 0x1f)
            g = 0x1f;
        if (bl > 0x1f)
            bl = 0x1f;
        if (r < 0)
            r = 0;
        if (g < 0)
            g = 0;
        if (bl < 0)
            bl = 0;
        *dst = (bl << 10) | (g << 5) | r;
        dst++;
        src++;
        i++;
    } while (i <= 0x3e);
    L2db8 += (__Random() & 7) * 3;
    if (L1f00[L2db8] == 0x63)
        L2db8 = 0;
}
