/* Cluster Func_80f2ebc..Func_80f2ebc extracted from goldensun/asm/rom_f2000/rom_f2028_c_c_a_a_a.s.
 *
 * Total .text for this TU = 84 bytes (= 0x54). Parked in an earlier batch; elevated in
 * batch 277. No pins, no flags, no symbol rename.
 *
 * NEAR-TWIN OF Func_809088c (src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_b.c), differing in
 * exactly ONE constant: the loop bound is 0x600 here and 0x540 there. Read that file for
 * the derivation -- in particular for why this park's "do not re-try the function-pointer
 * route" was wrong, and why `divsi3_RAM` needs no flag group and no per-object rename.
 * Each source was verified by objcmp against its OWN reference. If you edit one, check
 * the other.
 */
extern int divsi3_RAM(int a, int b);

void Func_80f2ebc(short *a, short *b, short *c, int n)
{
    int i;
    int u;
    int v;
    int (*f)(int, int);

    if (n > 0) {
        f = divsi3_RAM;
        for (i = 0; i < 0x600; i++) {
            u = *a;
            v = *b;
            *c = f(v - u, n);
            a++;
            b++;
            c++;
        }
    }
}
