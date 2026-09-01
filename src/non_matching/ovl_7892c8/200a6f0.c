/* OvlFunc_888_200a6f0 (0x0200a6f0) -- NON-MATCHING.
 * Blocker class: WHICH value goes in the high register.
 *
 * 43 lines against 45. Both use r8 and r10; they disagree about what goes
 * where, and the ROM pays for it:
 *
 *              the angle    the +0x64 pointer
 *     rom      r6 (low)     r8  -- so it costs `mov r2, r8` and `mov r1, r8`
 *     ours     r8           r6 (low) -- no moves needed
 *
 * The angle is used twice and early (both trig calls); the pointer is used
 * twice and late. gcc gives the cheap register to the longer-lived value and
 * comes out TWO INSTRUCTIONS SHORTER than the ROM. This is not a spelling
 * problem -- our code is better and there is nothing to write.
 *
 * MEASURED (rom 45 lines, all 43 / 30 differing):
 *   `q = a + 0x64;`
 *   `k = 0x64; q = a + k;`
 *   `q = (unsigned char *)(0x64 + (int)a);`
 *
 * The last two were aimed at the ROM's `mov r2, #0x64 / add r2, r5` (offset
 * materialised first) against our `mov r6, r5 / add r6, #0x64`. Both are two
 * instructions and neither spelling moves it; the difference follows from which
 * register the value is heading for, not from the arithmetic.
 *
 * WHAT IS RIGHT: `(c * 8 - c) << 1` and `(s * 4 + s) << 1` for the two trig
 * scalings; the re-read of `a->8` for the `a->0x38` store; and the pointer
 * advanced in place for the final halfword accumulate.
 *
 * NEXT: nothing source-level.
 */
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_888_200a6f0(unsigned char *a)
{
    unsigned char *q;
    unsigned char *b;
    int ang;
    int c;
    int s;
    int v;

    q = a + 0x64;
    ang = *(unsigned short *)q;
    b = *(unsigned char **)(a + 0x68);
    c = __cos(ang);
    *(int *)(a + 8) = *(int *)(b + 8) + ((c * 8 - c) << 1);
    s = __sin(ang);
    v = *(int *)(b + 0x10) + ((s * 4 + s) << 1);
    *(int *)(a + 0x10) = v;
    *(int *)(a + 0x40) = v;
    *(int *)(a + 0x38) = *(int *)(a + 8);
    a += 0x66;
    *(unsigned short *)q = *(unsigned short *)q + *(unsigned short *)a;
}
