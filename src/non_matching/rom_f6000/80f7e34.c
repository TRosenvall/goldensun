/* Func_80f7e34  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_f6000/rom_f6008_c.s
 * Best screen: 15 instructions in disagreeing regions, of 21 (rom 21, ours 24).
 *
 * BLOCKER CLASS: CSE across a possibly-aliasing store -- gcc caches, the ROM
 * reloads.
 *
 * A doubly-linked-list unlink. After writing through one of the two pointers,
 * the ROM reloads BOTH from memory:
 *
 *      str r0, [r2, #4]
 *      .L: ldr r2, [r1, r4] / ldr r3, [r1, r3] / str r3, [r2]
 *
 * gcc proves the store cannot have changed either slot and keeps the earlier
 * values in registers, adding `mov r5, r1` and `mov r0, r2` to hold them. Ours
 * is three instructions LONGER as a result -- the copies cost more than the
 * reloads saved.
 *
 * Reloading is written out explicitly below (the two assignments to `n` and `p`
 * after the store) and gcc removes them anyway. The only way to defeat that
 * analysis is `volatile`, which is a fakematch and is not used here -- see
 * reports/fakematch-worklist.md for why that debt is tracked rather than
 * incurred.
 *
 * The index arithmetic is correct: `k = i * 3; k <<= 2;` gives the ROM's
 * `lsl r3, r0, #1 / add r3, r0 / lsl r3, #2`, and `j = k + 4` gives the
 * separate `add r4, r3, #4`.
 */
extern unsigned char *ewram_2004c00;

void Func_80f7e34(int i)
{
    unsigned char *b;
    unsigned int k;
    unsigned int j;
    unsigned char *n;
    unsigned char *p;

    b = ewram_2004c00;
    k = i * 3;
    k <<= 2;
    j = k + 4;
    n = *(unsigned char **)(b + j);
    if (n == 0)
        return;
    p = *(unsigned char **)(b + k);
    if (p != 0)
        *(unsigned char **)(p + 4) = n;
    n = *(unsigned char **)(b + j);
    p = *(unsigned char **)(b + k);
    *(unsigned char **)n = p;
}
