/* Cluster Func_80f4100..Func_80f4100 extracted from goldensun/asm/rom_f4000/rom_f4008_c_c.s.
 *
 * Total .text for this TU = 104 bytes (= 0x68). Never attempted before batch 277.
 * No pins, no flags.
 *
 * ScalePalette. The hand-written `.s` this replaced carried a header comment naming it and
 * describing it, which is preserved here: r0 = source colours, r1 = destination, r2 = a
 * 16.16 scale, r3 = count; each 5:5:5 channel is extracted with one of the masks 0x001f,
 * 0x03e0 and 0x7c00, multiplied, shifted back down by 16 and re-masked, so a channel that
 * overflows truncates to its own field rather than carrying into the next.
 *
 * IDENTICAL TWIN OF Func_80f6038 (src/rom_f6000/rom_f6008_c_a_a.c) -- read that file for
 * the two levers, which are the accumulator-reuse register lever and `x *= scale;` as its
 * own statement. Each source was verified by objcmp against its OWN reference rather than
 * inferred from the other. If you edit one, edit both.
 *
 * NOTE ON THE SPLIT: the parent `.s` carried a `.rodata` section, so this needed a
 * text/data split rather than a plain one. It was clean because the data sits at the END of
 * the file, after `LuckyDiceMain`, and this function is FIRST -- so splitting it off the
 * front leaves the data with its own object untouched. Verified after the split: this piece
 * has one function and no rodata; the remainder has `LuckyDiceMain` and all of it.
 */
int Func_80f4100(unsigned short *src, unsigned short *dst, unsigned int scale, int n)
{
    unsigned int c;
    unsigned int b;
    unsigned int g;
    unsigned int r;
    int i;

    for (i = 0; i < n; i++) {
        c = *src;
        b = c & 0x1f;
        g = c & 0x3e0;
        r = c & 0x7c00;
        b *= scale;
        g *= scale;
        r *= scale;
        c = b >> 16 & 0x1f;
        c |= g >> 16 & 0x3e0;
        c |= r >> 16 & 0x7c00;
        *dst = c;
        src++;
        dst++;
    }
    return 0;
}
