/* Func_808ddb8  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_8a000/rom_8d9a4_a_a.s
 * Best screen: 12 instructions in disagreeing regions, of 26 (streams same length).
 *
 * BLOCKER CLASS: register allocation. THE STREAMS ARE STRUCTURALLY IDENTICAL --
 * every mnemonic lines up, including the duplicated loop preamble. Two
 * registers are exchanged and each of the six affected instructions is counted
 * twice because the preamble appears at both the entry and the loop bottom.
 *
 *      rom   mov r4, #0x0 / ldrsh r3, [r2, r4] / mov r4, #0x1 / neg r4, r4
 *            mov r1, #0x10
 *      ours  mov r1, #0x0 / ldrsh r3, [r2, r1] / mov r1, #0x1 / neg r1, r1
 *            mov r4, #0x10
 *
 * The ROM keeps the -1 sentinel in r4 and the default return 0x10 in r1; gcc
 * does the opposite. Both are live across the loop and the function makes no
 * calls, so caller-saved versus callee-saved does not decide it -- there is no
 * source-level handle on which name each value gets.
 *
 * WHAT IS ALREADY RIGHT and should not be re-derived:
 *   - The un-rotated `goto test;` loop with the preamble written out at BOTH
 *     the entry and the loop bottom. gcc will not duplicate it on its own; a
 *     plain while loop hoists it and the shape collapses.
 *   - The zero offset as a named local at each `ldrsh`, which Thumb requires
 *     since `ldrsh` has no immediate form.
 *   - `s = 1; s = -s;` rather than `s = -1`, matching `mov r4, #1 / neg r4, r4`.
 */
extern unsigned char L9e686[] __asm__(".L9e686");

int Func_808ddb8(int key)
{
    short *p;
    unsigned int o;
    int v;
    int s;
    int r;

    p = (short *)L9e686;
    o = 0;
    v = *(short *)((unsigned char *)p + o);
    s = 1;
    s = -s;
    r = 0x10;
    goto test;
loop:
    p++;
    o = 0;
    v = *(short *)((unsigned char *)p + o);
    s = 1;
    s = -s;
test:
    if (v == s)
        goto done;
    p++;
    if (key != v)
        goto loop;
    o = 0;
    r = *(short *)((unsigned char *)p + o);
done:
    return r;
}
