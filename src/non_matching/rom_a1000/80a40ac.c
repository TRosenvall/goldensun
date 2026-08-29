/* Func_80a40ac (DiscardFirstUnlockedItem) -- NON-MATCHING.
 * Blocker class: a POOLED 0x200 that no literal spelling reproduces.
 * 54 lines against the ROM's 55, 37 differing, and the whole opening -- the
 * call, the register-offset read, both zeroed counters, the pointer advance
 * and the entry branch -- is now exact.
 *
 * TWO LEVERS CARRIED IT FROM 45 TO 37 and both are already documented:
 *
 *   1. NAME THE OFFSET. The ROM reads the first slot with register-offset
 *      addressing, `mov r3, #0xd8 / ldrh r3, [r0, r3]`, and only afterwards
 *      advances the pointer by the same 0xd8 as an immediate. Naming 0xd8 in
 *      a variable for the read, while leaving the advance as a literal,
 *      produces both forms.
 *   2. AN INT FOR THE LOADED VALUE. With `unsigned short v` gcc emits `ldrsh`
 *      -- a signed load, equally valid for a `!= 0` test. Assigning the read
 *      to an `int` forces the zero-extending `ldrh` the ROM has. That is the
 *      same signedness family as the lsr/asr tell, on a load rather than a
 *      shift.
 *
 * WHAT REMAINS: the ROM POOLS the mask 0x200 -- `ldr r3, =0x200 / and r3, r2`
 * -- where gcc builds it with `mov` and `lsl`. This meets criterion 1 of
 * const.sym's bar for a named constant, and the attempts against criterion 2
 * are recorded here:
 *
 *   - `(v & 0x200)` with v an int:            mov r3, #0x80 / lsl
 *   - `(w & 0x200)` with w an unsigned short: 45 differing, WORSE -- gcc
 *     shifts to test the bit instead of masking, and the surrounding
 *     allocation moves with it
 *
 * The halfword-context exception recorded in const.sym does NOT apply: the
 * mask meets a value that has been widened to int by promotion, so there is no
 * HImode expression for gcc to pool into. If a third spelling is tried and
 * fails, this is a candidate for a const.sym entry -- but two attempts is not
 * the measured bar that file asks for.
 */
extern char *_GetUnit(int id);
extern int _Func_80788c4(int a, int b);

int Func_80a40ac(int who)
{
    char *u;
    unsigned short *p;
    int off;
    int v;
    int i, r, q, n;

    u = _GetUnit(who);
    off = 0xd8;
    v = *(unsigned short *)(u + off);
    r = 0;
    i = 0;
    p = (unsigned short *)(u + 0xd8);
    goto test;
body:
    v = *p;
    if ((v & 0x200) != 0)
        goto next;
    q = v >> 11;
    n = q + 1;
    if (q == 0)
        n = 1;
    if (n == 0)
        goto done;
    do {
        r = _Func_80788c4(who, i);
        n--;
    } while (n != 0);
done:
    if (r != 2)
        return 0;
    goto one;
next:
    i++;
    p++;
    if (i > 0xe)
        goto ret;
    v = *p;
test:
    if (v != 0)
        goto body;
one:
    r = 1;
ret:
    return r;
}
