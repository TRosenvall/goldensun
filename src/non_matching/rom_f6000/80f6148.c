/* Func_80f6148 (0x080f6148) -- NON-MATCHING.
 * Blocker class: register allocation ORDER, plus an inline constant pool.
 *
 * Darkens two palette regions by one step per channel. 76 lines against the
 * ROM's 78, and the two residues are independent.
 *
 * RESIDUE 1 -- THE TWO MISSING LINES ARE POOL SCAFFOLDING. Each loop's third
 * clamp ends like this in the ROM:
 *
 *     mov r4, #0x0
 *     b .Lf6184
 *         .align 2, 0
 *     .Lf617c: .word 0x1f
 *         .pool
 *     .Lf6184:
 *
 * That `b` jumps to the immediately following label. It is not control flow --
 * it is gcc emitting a literal pool in the middle of the function and inserting
 * a jump to step over it. Our version places its pools at the end, so the two
 * jumps do not exist and we come out exactly two lines short.
 *
 * WORTH READING OFF ANY DIFF: **a ROM `b` to the immediately-following label,
 * with an aligned `.word` between them, is a pool skip.** tryc.py already warns
 * that it normalises pool loads and cannot see where a pool sits; this is what
 * that warning costs in practice -- a two-line deficit that reads like a
 * missing statement and is not one. See also the `_CONST_1f` entry in
 * const.sym, which records a function in this class that screened at ONE
 * difference and still failed `make compare`.
 *
 * RESIDUE 2 -- ALLOCATION ORDER. The ROM keeps the walking pointer, the counter
 * and the mask in r5/r6/r7; gcc uses r0/r4/r5. THERE ARE NO CALLS IN THIS
 * FUNCTION, so nothing forces callee-saved registers and both choices are
 * equally valid -- gcc simply starts allocating at r0 and the original build
 * started higher. That is the REG_ALLOC_ORDER hypothesis recorded in HANDOFF,
 * and this function is the cleanest specimen of it yet: no calls, no spills, no
 * other residue, and every one of the 71 differing lines is the same three
 * registers renamed.
 *
 * MEASURED (rom 78 lines):
 *   plain `& 0x1f` on all three channels             74, 76
 *   `(int)&_CONST_1f` on the two SHIFTED channels
 *     and the literal on the third                   76, 71  <- best
 *
 * The `_CONST_1f` split is right and is what const.sym describes: the ROM has
 * `ldr r7, =0x1f` hoisted for the two shifted extractions and a separate
 * `mov r4, #0x1f` materialised inside the loop for the unshifted one. Written
 * with three plain literals gcc shares one register for all three and the
 * function is four lines short instead of two.
 *
 * WHAT IS RIGHT: the literal transcription `(((c << 16) >> 26) & mask) - 1` with
 * `c` UNSIGNED -- the `lsl #16` before each `lsr` is real and comes from the
 * shift pair being written that way, not from any cast; the three `cmp / bge /
 * mov #0` clamps; the `(r << 10) | (g << 5) | b` recombination; and the two
 * do/while loops with the counter incremented before the store.
 *
 * NEXT: nothing source-level. This one is worth re-screening first if anyone
 * ever rebuilds gcc with REG_ALLOC_ORDER starting at r4.
 */
extern int _CONST_1f;

void Func_80f6148(void)
{
    unsigned short *p;
    unsigned int c;
    int r;
    int g;
    int b;
    int i;

    p = (unsigned short *)0x5000140;
    i = 0;
    do {
        c = *p;
        r = (((c << 16) >> 26) & (int)&_CONST_1f) - 1;
        g = (((c << 16) >> 21) & (int)&_CONST_1f) - 1;
        b = (c & 0x1f) - 1;
        if (r < 0)
            r = 0;
        if (g < 0)
            g = 0;
        if (b < 0)
            b = 0;
        i++;
        *p = (r << 10) | (g << 5) | b;
        p++;
    } while (i != 0x10);
    p = (unsigned short *)0x5000202;
    i = 0;
    do {
        c = *p;
        r = (((c << 16) >> 26) & (int)&_CONST_1f) - 1;
        g = (((c << 16) >> 21) & (int)&_CONST_1f) - 1;
        b = (c & 0x1f) - 1;
        if (r < 0)
            r = 0;
        if (g < 0)
            g = 0;
        if (b < 0)
            b = 0;
        i++;
        *p = (r << 10) | (g << 5) | b;
        p++;
    } while (i != 0xef);
}
