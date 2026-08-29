/* Func_800fa8c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_9000/rom_f9cc_a_c.s
 * Best screen: 20 instructions in disagreeing regions, of 28 (streams same length).
 *
 * BLOCKER CLASS: register allocation. The structure is EXACT -- every mnemonic,
 * every immediate, every branch -- and the registers are shifted by one
 * throughout (ROM r4/r0/r5/r6 against our r0/r1/r4/r5).
 *
 * The underlying difference is that the ROM keeps four values in CALLEE-SAVED
 * registers and saves only three of them (`push {r5, r6, lr}` while also using
 * r4), where gcc uses two caller-saved and two callee-saved and saves what it
 * uses. That is the same shape seen across the register-allocation parks: the
 * ROM's allocator reaches for high registers sooner.
 *
 * Everything expressible in C is already right: the pointer walk is a
 * post-increment `*p++` giving `ldmia r5!, {r1}`, the store goes back to
 * `p - 1` after the walk, the mask is a named local so `m = v; m &= mask;`
 * keeps the ROM's `mov r2, r1 / and r2, r6`, and the count is built as
 * `n = 0x80; n <<= 7;`.
 */
extern unsigned int gBuffer[];

void Func_800fa8c(void)
{
    unsigned int *p;
    int mask;
    int n;
    int acc;
    int v;
    int m;
    int t;

    n = 0x80;
    acc = 1;
    p = gBuffer;
    mask = 0xfff;
    n <<= 7;
    acc = -acc;
    do {
        v = *p++;
        m = v;
        m &= mask;
        if (m == mask) {
            if (acc != m)
                acc++;
            t = v + acc;
            t = t - m;
            *(p - 1) = t;
        }
        n--;
    } while (n != 0);
}
