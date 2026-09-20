/* Cluster Func_80218dc..Func_8021950 extracted from goldensun/asm/rom_15000/rom_20198_c_c_c_a_a_c_c.s.
 *
 * Total .text for this TU = 236 bytes (= 0xec).
 * The .s held exactly these two functions and BOTH are elevated, so the whole file
 * becomes one translation unit and stage1.ld:512 is untouched -- no split at all.
 *
 * Func_80218dc was parked at 4 of 50; Func_8021950 was never attempted. No pins, no
 * flags. Each was verified on its own against the reference (116 bytes / 53
 * encodings and 120 bytes / 59 encodings) before being combined here, and the
 * combined unit is gated on make compare.
 *
 * Func_80218dc -- THE FIX IS ONE WORD: the callee is declared `void`, not `int`. The
 * park's four differing instructions were an argument rotation, and the recorded
 * table of argument orders names it: for an argument needing a precompute, a VOID
 * callee emits `r2, r0, r1` and an INT callee `r2, r1, r0`. The park had tried "the
 * no-prototype lever", which is the OPPOSITE change -- no declaration means an
 * implicit `int`. Third function this batch to turn on a callee's return type.
 *
 * NOTE ON ITS OWN RETURN TYPE, because it looks like a bug and is not: Func_80218dc
 * is declared `int` and FALLS OFF THE END. That is deliberate. gcc emits no
 * `mov r0, #0`, so the ROM's `pop {r1} / bx r1` returns whatever the last call left
 * in r0. Declaring it `void` adds nothing here but the ROM's tail is the
 * falls-off-the-end shape, so the `int` is what reproduces it.
 *
 * Func_8021950 -- a 4bpp nibble overlay. `b`'s row is shifted `shift` nibbles
 * (`-shift * 4` right when negative; both amounts are hoisted, into r8 and r14),
 * then eight nibbles are merged MSB-first with `b`'s nibble winning wherever its top
 * nibble is non-zero -- which is the pooled 0xfffffff and the `bls`. Both pointers
 * spill to sp+4/sp+0 under the pressure and are walked with `ldmia rN!`.
 *
 * ITS SINGLE LEVER IS THE POSITION OF `acc = 0`: it must sit BETWEEN the two pointer
 * loads. Measured: after both loads, 45 of 63 matched (the ldmia reload scratch takes
 * r3 instead of r0, and `i = 0`'s mov takes r2 instead of r0); before both, 56 of 63;
 * between them, EXACT. Also measured: `i != 8` gives 43 of 63, and splitting `*b; b++`
 * or `*out = acc; out++` gives 45.
 */
extern void Func_8019000(int a, int b, int c, int d, int e);


int Func_80218dc(int a, int b, int c, int d)
{
    int base;

    base = 0xf315 + d * 2;
    Func_8019000(a, (0x80 << 3) | base, b, c, 0);
    Func_8019000(a, 0xf314 + d * 2, b + 1, c, 0);
    Func_8019000(a, base, b + 2, c, 0);
}

void Func_8021950(unsigned int *a, unsigned int *b, unsigned int *out, int shift)
{
    int i, j;
    unsigned int x, y, acc;

    for (i = 0; i < 8; i++) {
        y = *b++;
        acc = 0;
        x = *a++;
        if (shift < 0)
            y >>= -shift * 4;
        else
            y <<= shift * 4;
        for (j = 7; j >= 0; j--) {
            acc <<= 4;
            if (y > 0xfffffff)
                acc += y >> 28;
            else
                acc += x >> 28;
            y <<= 4;
            x <<= 4;
        }
        *out++ = acc;
    }
}
