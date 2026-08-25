/* GetEnemyAttackAnimParam  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_c1a34_a_a_c_c_a_a.s
 * Best screen: rom 18 lines, ours 15 -- OURS IS THREE SHORTER.
 *
 * BLOCKER CLASS: provable redundancy that gcc removes.
 *
 * The ROM ends the table path with
 *
 *      mov r0, r3 / cmp r3, #0 / bne .Lexit / mov r0, #0 / .Lexit:
 *
 * That trailing `mov r0, #0` is DEAD: control only reaches it when r3 is zero,
 * and r0 already holds r3. gcc proves that and drops the instruction, and with
 * it the separate `mov r0, r3`.
 *
 * WHAT WAS TRIED
 *  1. `if (t == 0) return 0; return t;` -- the spelling that produces exactly
 *     this shape when the compiler does not fold it (kept below).
 *  2. The same with the bounds test inverted so the table path is the branch
 *     target, matching the ROM's `bls`.  No change to the length; the three
 *     missing instructions are the same three.
 *
 * This is the same family as the mask-narrowing parks: where the ROM contains a
 * provably redundant instruction, no source spelling reintroduces it, because
 * the optimiser removes it after the source has had its say. The only lever
 * that has ever worked on this class is a compiler flag, and there is none for
 * it here.
 */
extern unsigned char Lc7420[] __asm__(".Lc7420");

int GetEnemyAttackAnimParam(int i)
{
    unsigned char *p;
    int t;

    if ((unsigned int)i > 0xab)
        return 0;
    p = Lc7420 + (i << 3);
    t = p[2] >> 5;
    if (t == 0)
        return 0;
    return t;
}
