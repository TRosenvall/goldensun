/* Func_8092ba8  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_8a000/rom_92950_c_a_c_a.s
 * Best screen: 4 instructions in disagreeing regions, of 25 (rom 25, ours 24).
 *
 * BLOCKER CLASS: a copy elided at a shared exit -- the same shape as
 * src/non_matching/rom_15000/80170c4.c.
 *
 * The ROM computes the result in r1 throughout and copies it to r0 at the
 * single exit; gcc computes it in r0 directly and drops the `mov r0, r1`.
 * Everything else matches, including the sentinel built as `r = 1; r = -r;`
 * and the constant-as-destination AND, `m = 0xfff; m &= i;`, which reproduces
 * the ROM's `and r3, r0`.
 *
 * There is no source handle: the value IS the return value, and which register
 * gcc computes it in is not something C states.
 */
extern unsigned char *iwram_3001ebc;

int Func_8092ba8(int i)
{
    unsigned char *b;
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    unsigned char *t;
    int m;
    int r;

    b = iwram_3001ebc;
    m = 0xfff;
    m &= i;
    m <<= 2;
    m += 0x14;
    p = *(unsigned char **)(b + m);
    r = 1;
    r = -r;
    if (p == 0)
        goto out;
    q = p;
    q += 0x54;
    if (*q != 1)
        goto out;
    s = *(unsigned char **)(p + 0x50);
    t = *(unsigned char **)(s + 0x28);
    r = *(short *)(t + (unsigned int)0);
out:
    return r;
}
