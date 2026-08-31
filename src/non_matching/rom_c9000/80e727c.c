/* Func_80e727c  @ 0x080e727c  [rom_c9000]
 *
 * Source asm: goldensun/asm/rom_c9000/rom_e6638_a_c.s
 *
 * A palette-adjust loop: over 0x3f entries from 0x5000002, unpack the three
 * five-bit BGR channels, add a per-channel delta, clamp each to 0x1f, repack
 * and store.
 *
 * BLOCKER: how the 0x1f mask is materialised. 47 lines against 48, 34
 * differing, and the loop BODY is structurally correct -- `ldrh r2, [r5]`,
 * `lsl r3, r2, #0x10` and both extractions match in shape and position.
 *
 * THE READING THAT GOT IT THERE, and which should not be re-derived: the ROM
 * computes `v << 16` ONCE and derives two channels from it while the third
 * uses `v` directly. Writing the three extractions independently as
 * `(v >> 10) & 0x1f` etc. is 45 differing and forces gcc to mask in the
 * SHIFTED domain for the third channel (`mov r3, #0xf8 / lsl r3, #0xd`), which
 * is nothing like the ROM. Naming the shared shift --
 *
 *     t = v << 16;
 *     b = (t >> 26) & 0x1f;
 *     g = (t >> 21) & 0x1f;
 *     r = 0x1f & v;
 *
 * -- is 34, and takes the whole loop body to the right shape.
 *
 * WHAT IS LEFT is that the ROM holds 0x1f TWICE: pooled into callee-saved r7
 * before the loop (`ldr r7, =0x1f`, shared by the blue and green `and`s) and
 * again as a fresh `mov r0, #0x1f` inside the loop for the red channel. We
 * emit one `mov r6, #0x1f` and share it three ways, and every remaining
 * difference is the register cascade from that.
 *
 * A POOLED 0x1f IS ITSELF ODD -- it fits an eight-bit `mov`, so gcc had no
 * cost reason to pool it. Per docs/elevation.md a pooled small constant is
 * normally a SYMBOL tell, but a five-bit colour mask is an unlikely symbol.
 * The other explanation on record is operand mode (batch 155), i.e. that the
 * mask sits in an HImode context somewhere. Neither is confirmed here.
 *
 * MEASURED:
 *   shared `t = v << 16`, literals for all three masks        34  (best)
 *   three independent shifts, `unsigned short` value          45
 *   ... with a `volatile` pointer                             43
 *   ... with the value typed `int`                            47  (42 lines)
 *   shared named mask for blue/green, literal for red         50  (51 lines)
 *
 * The last is the batch-153 constant-remat lever aimed at exactly this shape
 * and it goes the wrong way, which is worth knowing before anyone tries it
 * again here.
 *
 * The pool shape is NOT the blocker: this is a branch-over-pool function and
 * the `b` over the pool is in our output at the ROM's position throughout.
 */
void Func_80e727c(int db, int dg, int dr)
{
    unsigned short *p;
    unsigned int v;
    unsigned int t;
    int i;
    int b;
    int g;
    int r;

    p = (unsigned short *)0x5000002;
    i = 0;
    do {
        v = *p;
        t = v << 16;
        b = (t >> 26) & 0x1f;
        g = (t >> 21) & 0x1f;
        r = 0x1f & v;
        b += db;
        g += dg;
        r += dr;
        if (b > 0x1f)
            b = 0x1f;
        if (g > 0x1f)
            g = 0x1f;
        if (r > 0x1f)
            r = 0x1f;
        i++;
        *p = (b << 10) | (g << 5) | r;
        p++;
    } while (i != 0x3f);
}
