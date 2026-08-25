/* Func_80bf54c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 * Best screen: 4 instructions in disagreeing regions, of 19 (rom 19, ours 18).
 *
 * BLOCKER CLASS: register allocation -- ONE elided copy, and the rest is the
 * one-slot shift it causes.
 *
 *      rom   add r1, r0, r3 / ldrb r2, [r1] / mov r3, r2 / cmp r3, #0
 *      ours  add r2, r0, r3 / ldrb r3, [r2] /             cmp r3, #0
 *
 * The ROM loads the byte into one register and COPIES it before testing. gcc
 * loads straight into the register it will modify and skips the copy, so our
 * stream is one instruction shorter and every label after it shifts.
 *
 * WHAT WAS TRIED, all four byte-identical to each other:
 *  1. `v = *q; t = v;` -- two named locals, which is the spelling that DOES
 *     produce the copy in this function's sibling Func_80bf574 (see
 *     src/non_matching/rom_b5000/80bf574.c, whose stream contains the copy).
 *  2. `t = *q;` with no intermediate at all.
 *  3. The declaration lever: `q` declared before `p`.
 *  4. A derived initialiser for `q`, `p + 0x13f` inline instead of through a
 *     named offset -- the lever that landed Func_80a9cbc in this same batch.
 *
 * Why (1) works in the sibling and not here is the useful part: Func_80bf574
 * has a SECOND store through the same pointer at a different offset, so more
 * values are live at once and gcc needs the extra register anyway. Here the
 * pointer is used twice and nothing else competes, so the copy is pure waste
 * and gcc removes it. The copy is a symptom of pressure, not of spelling --
 * which is the same conclusion reached for the dead r8 in
 * src/non_matching/ovl_7bf5a8/2008704.c.
 *
 * The `lsl r3, #24` with NO following `lsr` is reproduced by `t = t << 24;`
 * as its own statement before the test; that part is right.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf54c(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int off;
    int v;
    int t;
    int r;

    p = _GetUnit();
    off = 0x13f;
    q = p + off;
    v = *q;
    t = v;
    if (t == 0)
        goto zero;
    t = t + 0xff;
    *q = t;
    t = t << 24;
    r = 1;
    if (t == 0)
        goto out;
zero:
    r = 0;
out:
    return r;
}
