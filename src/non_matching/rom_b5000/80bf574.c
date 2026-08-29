/* Func_80bf574  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 * Best screen: 8 instructions in disagreeing regions, of 25 (streams same length).
 *
 * BLOCKER CLASS: constant-CSE.  The two byte offsets this function uses are
 * 0x146 (written `0xa3 << 1`, which is how the ROM builds it: `mov r3, #0xa3 /
 * lsl r3, #1`) and 0x147.  They differ by one, and gcc SEES that:
 *
 *      rom   ldr r1, =0x147        <- fresh from the literal pool
 *      rom   add r2, r0, r1
 *      ours  add r0, #0x1          <- derived from the 0x146 already in hand
 *      ours  add r2, r1, r0
 *
 * Because gcc wants r0 for that arithmetic, it also copies the _GetUnit result
 * out of r0 into r1 (`mov r1, r0`), which the ROM never does, and the copy
 * drags a further register renaming through the rest of the body.
 *
 * WHAT WAS TRIED
 *
 *  1. Naming both offsets as locals (`k = 0xa3 << 1; off = 0x147;`).  8 of 25.
 *  2. Inlining both offset expressions with no locals at all.  Identical
 *     output, 8 of 25 -- gcc relates the two constants either way.
 *
 * THREE SPELLINGS THAT DID EACH FIX A REAL PIECE and are kept below, since
 * without them this sits at 17 of 25:
 *
 *  - Shared-exit `goto fail`, not two `return 0`s.  The ROM has one epilogue
 *    reached by `mov r0, #1 / b L1` from the success path and by falling into
 *    `L0: mov r0, #0` from both failures.
 *  - `t = t + 0xff` rather than `t = t - 1`.  The ROM's `add r3, #0xff` is not
 *    a peephole for the subtract; writing the subtract emits `sub r2, #0x1`.
 *  - Narrowing IN PLACE as its own statement, `t = (unsigned char)t;` before
 *    the test.  Written as a cast inside the condition, the narrowed value
 *    becomes a new temporary and gcc emits the three-operand
 *    `lsl r3, r2, #0x18` where the ROM has the destructive `lsl r3, #0x18`.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf574(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    unsigned int k;
    unsigned int off;
    int v;
    int t;

    p = _GetUnit();
    k = 0xa3 << 1;
    q = p + k;
    v = *q;
    t = v;
    if (t == 0)
        goto fail;
    t = t + 0xff;
    *q = t;
    t = (unsigned char)t;
    if (t != 0)
        goto fail;
    off = 0x147;
    r = p + off;
    *r = t;
    return 1;
fail:
    return 0;
}
