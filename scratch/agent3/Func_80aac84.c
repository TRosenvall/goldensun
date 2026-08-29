/* Func_80aac84 (BrightenPaletteBank) -- PARKED.  ref: asm/rom_a1000/rom_aa538_c_c_a.s
 *
 * Default flags: 73 differing of 77 (ours 76 lines), first diff at position 1.
 * With --cflags "-fno-strength-reduce": 79 lines, first diff at 2, and the
 * structure matches (the ROM's `mov r2,r14 / add r3,r2,r6 / lsl r0,r3,#1`
 * reappears).  Two independent blockers remain and neither closed:
 *
 *  1. STRENGTH REDUCTION.  gcc turns the inner walk into `lsl r0, r3, #5` with
 *     `i` counting DOWN from 0xf; the ROM recomputes `(base + i) << 1` every
 *     iteration and keeps `base` in r14.  Only -fno-strength-reduce reaches it
 *     -- a `for` loop, a `do/while`, and a separately named `base` local are
 *     all identical to each other at 73 of 77.  That would be a NEW per-file
 *     flag group (the Makefile has only CSE_CFLAGS and GCSE_CFLAGS today), so
 *     it needs the coordinator's decision.
 *  2. The red/green/blue triple is ROTATED.  The ROM has red=r4, green=r2,
 *     blue=r1; we get red=r2, green=r1, blue=r4, in the same source order and
 *     with the same instructions.  Three orderings of the three extractions,
 *     reversing the three `+= delta`, and reversing the declarations were all
 *     measured at 60-72 of 77.  The ROM also groups the three `add rX, r5`
 *     together before the first clamp where gcc interleaves them with the mask
 *     extraction, which is the same allocation seen as scheduling.
 *
 * WHAT IS ALREADY RIGHT: `unsigned int c` (without it the two extractions are
 * `asr` where the ROM has `lsr`); `b = 0x1f & c;` written constant-first, which
 * gives the ROM's `mov r1, r7 / and r1, r3` while red and green correctly give
 * `and r4, r7` / `and r2, r7`; and `idx = 5` in the else arm with `idx = 7` and
 * `delta = -12` in the then arm, which reproduces the ROM's speculated
 * `mov r3,#5` above the `cmp`.
 * Note the ROM really does use idx 0xf, then 5, then 7 -- the header comment on
 * the .s claiming passes 2 and 3 both use 0x0e0 is wrong; pass 2 uses 5<<4.
 */
void Func_80aac84(int delta)
{
    int pass;
    int idx;
    int i;
    int off;
    unsigned int c;
    int r;
    int g;
    int b;

    pass = 0;
    idx = 0xf;
    do {
        idx <<= 4;
        i = 0;
        do {
            off = (idx + i) << 1;
            c = *(unsigned short *)(off + (0xa0 << 19));
            r = (c >> 10) & 0x1f;
            g = (c >> 5) & 0x1f;
            b = 0x1f & c;
            r += delta;
            g += delta;
            b += delta;
            if (r > 0x1f)
                r = 0x1f;
            if (g > 0x1f)
                g = 0x1f;
            if (b > 0x1f)
                b = 0x1f;
            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
            *(unsigned short *)(off + 0x4ffffe0) = (r << 10) | (g << 5) | b;
            i++;
        } while (i <= 0xf);
        if (pass != 0) {
            delta = -12;
            idx = 7;
        } else {
            idx = 5;
        }
        pass++;
    } while (pass <= 2);
}
