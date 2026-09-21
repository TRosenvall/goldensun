/* Cluster Func_80f6038..Func_80f6038 extracted from goldensun/asm/rom_f6000/rom_f6008_c_a.s.
 *
 * Total .text for this TU = 104 bytes (= 0x68). Never attempted before batch 277.
 * No pins, no flags. This file needs no split -- the function is alone in its `.s`.
 *
 * ScalePalette: scales each 5:5:5 channel of a palette by a 16.16 factor, re-masking each
 * channel so an overflow truncates to its own field rather than carrying into the next.
 * (The name and that description come from the disassembly's own header comment on the
 * identical twin, which is worth knowing exists.)
 *
 * IDENTICAL TWIN: Func_80f4100, at src/rom_f4000/rom_f4008_c_c_b.c -- 54 normalised lines,
 * identical. One source with the name changed matches both, and each was verified by
 * objcmp against its OWN reference. Found with tools/dupfuncs.py. If you edit one, edit
 * both.
 *
 * TWO LEVERS, and the first is new.
 *
 * 1. REUSING THE SOURCE VARIABLE AS THE ACCUMULATOR IS A REGISTER LEVER -- worth 29 of 54.
 *    Everything except the register NAMES was already right at 29 differing: our counter
 *    took r4 and the loaded colour r0, where the ROM has the counter in r0 and the colour
 *    in r4, and the three channel registers rotated with them. `local_alloc` priority is
 *    roughly references over live length, so a short-lived `c` is the HIGHEST-priority
 *    local and grabs the first register, which forces the global counter out to r4.
 *    Writing the recombination back into `c` -- `c = b >> 16 & 0x1f; c |= ...; *dst = c;`
 *    -- stretches `c` across the whole body, drops it to the LOWEST priority, and the
 *    ROM's whole map falls out.
 *
 *    GENERAL FORM: when a ROM reuses a dead value's register as an accumulator, suspect
 *    ONE variable rather than two. This is the mirror of "distinct call results want
 *    distinct variables" -- here a source and an accumulator that look like two things
 *    are one.
 *
 * 2. `x *= scale;` AS ITS OWN STATEMENT, not `x = (c & m) * scale;`. Separate statements on
 *    one variable are ONE pseudo, so the AND's destination register is already the `mul`'s
 *    destination and Thumb's earlyclobber `mul rD, rS` needs no copy (`mul r3, r5`). The
 *    single-statement form makes two pseudos and costs one `mov rD, scale` per channel.
 *    38 -> 31 on its own. Same family as the recorded destructive-`mulsi3` rule.
 *
 * INERT, all at 54 lines and 38 differing -- do not re-try: any declaration order of
 * c/b/g/r/i; `int` against `unsigned int` for the channels; mask-operand order
 * (`0x1f & c` against `c & 0x1f`); a `ret` local for the `return 0`.
 * ACTIVELY WORSE: an `unsigned` counter (60 lines); `unsigned short c` (55/45); the masks
 * as named locals (55/49); array indexing (52 lines, but a single-index-register shape the
 * ROM does not have); named products `b = (c & m) * scale` (64/61).
 */
int Func_80f6038(unsigned short *src, unsigned short *dst, unsigned int scale, int n)
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
