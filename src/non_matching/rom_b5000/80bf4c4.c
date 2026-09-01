/* Func_80bf4c4 (0x080bf4c4) -- NON-MATCHING.
 * Blocker class: copy-then-modify, reachable at one site and not the other.
 *
 * 47 lines against the ROM's 47, TWENTY-FIVE differing. Four readings landed
 * and the file below carries all of them; what is left is one register copy.
 *
 * 1. THE COUNTER IS `unsigned int`, NOT `unsigned char`. The ROM decrements
 *    with `add r3, #0xf8` and `add r3, #0xff` -- ADDING 248 and 255, not
 *    subtracting -- and masks to a byte exactly ONCE at the end
 *    (`lsl r3, r2, #24 / lsr r3, #24`). An `unsigned char` local makes gcc
 *    truncate after every store, which is two extra instructions per site;
 *    an `int` gets `sub r2, #8` and a signed `ble` where the ROM has `bls`.
 *    `unsigned int` with `v = v + 0xf8` gives the ROM's spelling on both.
 *
 * 2. THE COPY-THEN-MODIFY TELL IS REAL AT SITE 2. The ROM's
 *    `mov r3, r2 / add r3, #0xff / strb r3 / mov r2, r3` is a separate named
 *    value, not `v = v + 0xff; *p = v;`. Writing `t = v + 0xff; *p = t; v = t;`
 *    matches that site instruction for instruction.
 *
 * 3. IT DOES NOT APPLY AT SITE 1. The same spelling there OVERSHOOTS to 49
 *    lines, because the ROM's site 1 is three instructions, not four -- r3
 *    already holds the value from the entry compare's `mov r3, r2`. Routing
 *    the entry tests through the scratch copy to reproduce that is also 49.
 *    So the tell has to be read per site against the instruction COUNT, not
 *    applied wherever the shape appears.
 *
 * 4. BRANCH POLARITY AT THE LAST TEST. `if (f(...) == 0) return 0;` puts the
 *    zero in-line and branches over it; the ROM branches to a SHARED
 *    `mov r0, #0` that three paths reach and lets the success block fall
 *    through. Writing the success arm as the `if` body -- `if (f(...) != 0)
 *    { *p = 0; return 1; } return 0;` -- is 29 differing to 25.
 *
 * MEASURED: unsigned char v 49/41, int v 45/39, unsigned int v 45/38,
 * copy at both sites 49/41, copy at site 2 only 47/29, plus the polarity
 * fix 47/25.
 *
 * NEXT: site 1's third instruction. The ROM's entry `mov r3, r2` and site 1's
 * `mov r2, r3` are a matched pair around a value that lives in two registers
 * at once; nothing measured here produces exactly one of them.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf4c4(int id)
{
    unsigned char *p;
    unsigned int v;
    unsigned int w;
    unsigned int t;

    p = _GetUnit(id) + 0x13d;
    v = *p;
    if (v == 0)
        return 0;
    if (v > 7) {
        v = v + 0xf8;
        *p = v;
    }
    if ((v & 7) != 0) {
        t = v + 0xff;
        *p = t;
        v = t;
    }
    w = (unsigned char)v;
    if (w == 0)
        return 1;
    if (w > 7)
        return 0;
    if (Func_80bf208(id, *p, 0x1e) != 0) {
        *p = 0;
        return 1;
    }
    return 0;
}
