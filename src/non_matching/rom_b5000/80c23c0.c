/* Func_80c23c0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_c1a34_a_a_c_c_a_a.s
 * Best screen: rom 20 lines, ours 15 -- OURS IS FIVE SHORTER.
 *
 * BLOCKER CLASS: gcc computes the same boolean more cheaply.
 *
 * The ROM extracts bit 0 with a branch:
 *
 *      lsl r3, #0x1f / mov r1, #0x0 / cmp r3, #0 / beq .L / mov r1, #0x1
 *      .L: mov r0, r1
 *
 * gcc extracts it with a shift pair:
 *
 *      lsl r0, #0x1f / lsr r0, #0x1f
 *
 * Two instructions against six, and identical in effect. `r = 0; if (t != 0)
 * r = 1;` -- written below, and exactly the boolean the ROM materialises --
 * is what gcc turns into the shift pair.
 *
 * This is the same family as GetEnemyAttackAnimParam in
 * src/non_matching/rom_b5000/80c2410.c and the mask-narrowing parks: where our
 * output is SHORTER because the optimiser proved something, no source spelling
 * puts the longer form back. The statement-level-branch lever (batch 53/55)
 * works when gcc rewrites CONTROL FLOW that the source can re-impose; it does
 * nothing when the rewrite is a value computation, as here.
 *
 * Its sibling Func_80c23a0 is parked separately at 4 of 16 for a different
 * reason -- register naming -- and shares only the table walk.
 */
extern unsigned char Lc7420[] __asm__(".Lc7420");

int Func_80c23c0(int i)
{
    unsigned char *p;
    int t;
    int r;

    if ((unsigned int)i > 0xab)
        return 0;
    p = Lc7420 + (i << 3);
    t = p[2];
    t <<= 31;
    r = 0;
    if (t != 0)
        r = 1;
    return r;
}
