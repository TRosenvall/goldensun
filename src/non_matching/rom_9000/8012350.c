/* Func_8012350 -- 0x08012350 -- asm/rom_9000/rom_1219c_a_c_c_a.s
 *
 * BLOCKER: loop rotation plus a folded bound. 24 of 27, two lines short.
 *
 * Polls two fields until both fall to 0xff or a frame cap is reached, waiting
 * a frame per pass, then clears a third field.
 *
 * TWO DIFFERENCES, both structural rather than register choice:
 *
 *   1. THE ROM PEELS THE FIRST LOAD. It loads p[1] BEFORE the loop and `b`s to
 *      the test, re-loading p[1] on the back edge; ours enters at the top and
 *      loads inside. That is the rotation the ROM's `b L0` announces.
 *
 *   2. THE BOUND IS BUILT AT RUNTIME. The ROM has `mov r3, #0x96 / lsl r3, #1`
 *      -- 0x12c in two instructions -- and compares `bge`. gcc rewrites
 *      `i >= 0x96 * 2` into `i > 0x12b` and POOLS 0x12b, because 0x12c will not
 *      fit an 8-bit immediate and the strength-reduced form saves nothing.
 *
 * MEASURED on the bound:
 *   `i >= 0x96 * 2` inline                     24 lines, 24 differ
 *   the bound named in a local, assigned at
 *     the top of the function                  25 lines, 26 differ
 *   the bound named in a local, assigned
 *     inside the loop before the test          25 lines, 24 differ  <- kept
 *
 * Naming the bound recovers one of the two missing lines but not the `>=`
 * against 0x12c: gcc applies the comparison rewrite after the local is folded,
 * so the pooled 0x12b survives. That is worth recording next to the
 * operand-mode rule, which DOES yield to an int local -- here the constant is
 * not the problem, the COMPARISON is, and no spelling of the bound changes
 * which relational operator gcc chooses.
 *
 * NOT TRIED: restructuring the loop to peel the first load by hand, which
 * would mean writing the condition twice. That is worth an attempt next time
 * -- docs/elevation.md records `while` versus rotated-`do` shapes deciding
 * exactly this kind of peel -- but it changes the source shape enough that it
 * should be measured, not assumed.
 */
extern int iwram_3001e70;
extern void WaitFrames(int n);

void Func_8012350(void)
{
    int *p;
    int i;
    int lim;

    p = (int *)iwram_3001e70;
    i = 0;
    while (p[1] > 0xff || p[2] > 0xff) {
        WaitFrames(1);
        i++;
        lim = 0x96 * 2;
        if (i >= lim)
            break;
    }
    p[3] = 0;
}
