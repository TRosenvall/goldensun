/* Func_80216b4  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_20198_c_c_c_a_a_c_a.s
 * Best screen: 14 instructions in disagreeing regions, of 22 (streams same length).
 *
 * BLOCKER CLASS: load ordering within an addition, plus the register naming it
 * drags along.
 *
 *      rom   ldrb r2, [r0, #0x8] / ldrb r3, [r5, r3] / add r2, r3
 *      ours  ldrb r3, [r4, r3]   / ldrb r5, [r0, #0x8] / add ...
 *
 * The ROM loads the struct byte first and the table byte second; gcc does the
 * reverse, even though the source adds them in the ROM's order.
 *
 * WHAT WAS TRIED
 *   1. `x = a[8]; x += tb[v];` -- source order matching the ROM (kept below).
 *      14 of 22.
 *   2. Both loads through separate named locals, `x = a[8]; y = tb[v];
 *      x += y;`, so neither is a subexpression of the other. WORSE, 18 of 22 --
 *      the extra local costs a register and perturbs both halves of the
 *      function.
 *
 * Attempt (2) is worth recording because naming subexpressions is usually the
 * safe move; here it is not. The function has two near-identical halves and any
 * extra live value is paid for twice.
 */
extern unsigned int iwram_3001800;
extern unsigned char L37226[] __asm__(".L37226");

void Func_80216b4(unsigned char *a)
{
    unsigned int *w;
    unsigned char *tb;
    unsigned char *b;
    unsigned int v;
    int m;
    int x;

    w = &iwram_3001800;
    v = *w;
    tb = L37226;
    m = 7;
    v >>= 2;
    v &= m;
    x = a[8];
    x += tb[v];
    a[0x14] = x;
    v = *w;
    v >>= 2;
    b = *(unsigned char **)a;
    v &= m;
    x = b[8];
    x += tb[v];
    b[0x14] = x;
}
