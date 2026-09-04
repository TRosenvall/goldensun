/* OvlFunc_897_200935c -- 0x0200935c
 *
 * The lightning timer: count down a hold, then count down a phase counter, and
 * when it expires pick a new phase at random and set both the flash mask and a
 * fresh hold length from it.
 *
 * NAMING AN INTERMEDIATE IS THE LEVER HERE, which is the INVERSE of the rule
 * batch 216 added. Each switch arm stores through the same global, and written
 * directly -- `*(unsigned int *)L3b70 = ...` -- gcc computes the address into a
 * scratch register AFTER the `bl __Random` in every arm. That makes each arm's
 * store textually identical to the early-return path's store, so cross-jumping
 * merges them into one shared block and the function comes out two lines short
 * (85 against 87, 81 differing -- almost all of it the resulting one-line
 * offset rather than 81 real disagreements).
 *
 * A named pointer local per arm buys the address a register of its own, which
 * puts it in callee-saved r5 and materialises it BEFORE the call, as the ROM
 * does in all four arms -- and, because the register now differs, the merge
 * that cost the two lines no longer happens.
 *
 * So the two rules are one rule with a test: name an intermediate when the
 * value must SURVIVE something (a call, or a register choice you need); do not
 * name one whose only role is to be consumed immediately. Batch 216's
 * OvlFunc_948_200a290 is the other side of exactly this.
 *
 * The rest is ordinary: `unsigned int __Random(void)` is what makes all five
 * `lsr #16` logical rather than arithmetic, the multipliers are written as
 * plain positive constants so they synthesise to the ROM's shift/add chains,
 * and the default arm's limit is spelled `(0xa0 << 1)` rather than `0x140`
 * because the ROM builds it with `mov`/`lsl`.
 */
extern unsigned int __Random(void);

extern unsigned char L3b68[] __asm__(".L3b68");
extern unsigned char L3b6c[] __asm__(".L3b6c");
extern unsigned char L3b70[] __asm__(".L3b70");

void OvlFunc_897_200935c(void)
{
    unsigned int *p;

    if (*(unsigned int *)L3b70 != 0) {
        *(unsigned int *)L3b70 -= 1;
        return;
    }

    if (*(unsigned int *)L3b6c != 0)
        *(unsigned int *)L3b6c -= 1;
    else
        *(unsigned int *)L3b6c = __Random() * 4 >> 16;

    switch (*(unsigned int *)L3b6c) {
    case 3:
        *(unsigned int *)L3b68 = 3;
        p = (unsigned int *)L3b70;
        *p = (__Random() * 20 >> 16) + 0x28;
        break;
    case 2:
        *(unsigned int *)L3b68 = 0xf;
        p = (unsigned int *)L3b70;
        *p = (__Random() * 40 >> 16) + 0x50;
        break;
    case 1:
        *(unsigned int *)L3b68 = 0x3f;
        p = (unsigned int *)L3b70;
        *p = (__Random() * 80 >> 16) + 0xa0;
        break;
    default:
        *(unsigned int *)L3b68 = 0x7f;
        p = (unsigned int *)L3b70;
        *p = (__Random() * 160 >> 16) + (0xa0 << 1);
        break;
    }
}
