/* Func_8026e80 (0x08026e80) -- NON-MATCHING.
 * Blocker class: basic-block LAYOUT -- gcc emits a nested test inline where the
 * ROM places it out of line, before the block that reaches it.
 *
 * 141 lines against the ROM's 144. The ROM's flow through the middle is:
 *
 *     .L26ef8  (the 0x56 / 0x53 test and the assignment)   <- EARLIER address
 *     .L26f0e  (the n < 0 test, which BRANCHES BACK to .L26ef8)
 *     .L26f34  (the join)
 *
 * so the inner test sits physically before the block that jumps to it. gcc lays
 * both out in source order however the source is written.
 *
 * MEASURED (rom 144 lines):
 *   the four conditions as one `&&` chain                     142, 125
 *   `t--` instead of `t = -1` inside `if (t == 0)`, so the
 *     ROM's `sub r0, #0x1` derives -1 from the live zero      141, 94
 *   `r5 = p; r6 = p; r5 += 0x24;` instead of
 *     `r5 = p + 0x24; r6 = p;`                                141, 87  <- best
 *   the inner test as a NESTED `if` rather than part of the
 *     `&&` chain                                              141, 87 (inert --
 *                          gcc flattens nested ifs into the same chain)
 *
 * THE `t--` RESULT IS THE REUSABLE ONE. Inside `if (t == 0)` the ROM does not
 * materialise -1; it decrements the zero that is already in the register.
 * Writing `t = -1;` gives `mov r0, #1 / neg r0, r0`; writing `t--;` gives the
 * ROM's single `sub r0, #0x1`. gcc does NOT constant-propagate `t == 0` into
 * the branch, so the decrement survives as a decrement. **When a ROM derives a
 * small constant from a value a comparison has just proved, write the
 * derivation, not the constant.**
 *
 * WHAT IS RIGHT: the `d / 3` step-toward helper with its zero fixup; the
 * three-iteration countdown loop with its counter spilled across the call; the
 * `(n + 0x3b) / 0x3c` and `s * 0x3c == n` minute test; and the five-argument
 * `Func_801ea08` call whose third argument is the value the preceding `cmp`
 * left in r2.
 *
 * NEXT: nothing found for the layout.
 */
extern unsigned char *iwram_3001f34;
extern unsigned char *iwram_3001e74;
extern unsigned char ewram_2002024[];
extern void _Func_80b8fd4(void);
extern void Func_8003dec(unsigned char *a, int b);
extern void Func_80219c8(void *a);
extern int Func_8021c34(void);
extern void _PlaySound(int id);
extern void Func_801ea08(int a, int b, int c, int d, int e);

void Func_8026e80(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *e;
    unsigned char *r5;
    unsigned char *r6;
    int a;
    int b;
    int d;
    int t;
    int n;
    int s;
    int i;

    p = iwram_3001f34;
    if (p == 0)
        return;
    a = *(int *)(p + 0x28);
    b = *(int *)(p + 0x2c);
    if (a != b) {
        d = a - b;
        t = d / 3;
        if (t == 0) {
            t--;
            if (d >= 0)
                t = 1;
        }
        *(int *)(p + 0x2c) = b + t;
        _Func_80b8fd4();
    }
    r5 = p;
    r6 = p;
    r5 += 0x24;
    i = 2;
    do {
        if (*r5++ != 0)
            Func_8003dec(r6, 0xf0);
        i--;
        r6 += 0xc;
    } while (i >= 0);
    Func_80219c8((void *)0x6006680);
    if (*(int *)(p + 0x50) == 0)
        return;
    q = iwram_3001e74;
    if (*(q + 0x52) != 0) {
        *(int *)(p + 0x4c) = 0;
        return;
    }
    n = *(int *)(p + 0x4c);
    if (n < 0) {
        t = *(q + 0x50);
        e = ewram_2002024 + (((t ^ 1) * 3) << 3);
        if (*(unsigned short *)(e + 8) == 0x45 && *(unsigned short *)(e + 0xa) == 0x44
            && *(unsigned short *)(q + 0xc) == 0x56 && *(unsigned short *)(q + 0xe) == 0x53) {
            *(int *)(p + 0x4c) = 0xe1 << 2;
            n = 0xe1 << 2;
        }
        if (n < 0)
            return;
    }
    if (*(int *)(p + 0x44) == 0 && *(int *)(p + 0x48) == 0) {
        *(int *)(p + 0x44) = Func_8021c34();
        n = *(int *)(p + 0x4c);
    }
    if (n > 0) {
        n = n - 1;
        *(int *)(p + 0x4c) = n;
    }
    if (n < 0)
        return;
    s = (n + 0x3b) / 0x3c;
    if (s != 0 && s * 0x3c == n)
        _PlaySound(0x6c);
    if (*(int *)(p + 0x44) != 0)
        Func_801ea08(s, 2, *(int *)(p + 0x44), 0x10, 8);
}
