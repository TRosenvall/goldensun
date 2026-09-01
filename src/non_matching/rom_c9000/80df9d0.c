/* Func_80df9d0 (0x080df9d0) -- NON-MATCHING.
 * Blocker class: where gcc parks a loop bound in a callee-saved register.
 *
 * 38 lines against the ROM's 38, TWO differing, and both are one instruction
 * pair swapped:
 *
 *     rom    mov r8, r3 / mov r5, #0x0        bound parked, then srcoff = 0
 *     ours   mov r5, #0x0 / mov r8, r3
 *
 * `r8` holds the outer loop's bound (0x90 << 1) and `r5` is the source offset.
 * Everything else -- both loops, the signed /2 idiom in each, the r12/r14
 * allocation for the two pointer parameters -- is exact.
 *
 * TWO LEVERS GOT IT FROM 5 TO 2, and one candidate lever made it worse. The
 * order matters, so they were measured one at a time after changing all three
 * together scored 9 -- worse than the 5 it started at.
 *
 *   baseline                                                    5 differing
 *   NAMED loop bound (`limit = 0x90 << 1;` before `srcoff = 0`) 11  WORSE
 *   offset-first pointer, `(unsigned char *)(srcoff + (int)src)` 4
 *   split load/increment/store instead of `dst[...] = *p++;`     3
 *   the last two together                                        2
 *
 * The named bound is the interesting negative. The residue LOOKS like the
 * recorded initialiser-order shape -- two setup instructions in the wrong
 * order -- and that rule has closed three functions. It does not apply when the
 * value in question is a loop BOUND that gcc hoists on its own: naming it gives
 * gcc a variable to place rather than a constant to rematerialise, and it
 * places it worse. `0x120` written as a literal instead of `0x90 << 1` is
 * byte-identical, so the spelling of the bound is not the variable either.
 *
 * Also measured, both 3 differing: `srcoff = 0;` before `outer = 0;`, and
 * `srcoff` written as a declaration initialiser.
 *
 * NEXT: nothing source-level. This is where gcc chooses to materialise a
 * hoisted loop bound, and the four spellings above do not reach it.
 */
void Func_80df9d0(unsigned char *src, unsigned char *dst, int stride)
{
    int outer;
    int srcoff;
    int j;
    int base;
    int idx;
    int v;
    unsigned char *p;

    outer = 0;
    srcoff = 0;
    do {
        base = srcoff / 2;
        j = 0;
        p = (unsigned char *)(srcoff + (int)src);
        do {
            idx = base + j / 2;
            v = *p;
            j++;
            p++;
            dst[idx] = v;
        } while (j != 0x28);
        outer++;
        srcoff += stride;
    } while (outer != (0x90 << 1));
}
