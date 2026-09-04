/* Func_807a7a0 -- 0x0807a7a0, asm/rom_77000/rom_79460_c_c_c_c_c_c.s
 *
 * Restores a saved party from the staging buffer at ewram_2001078, but only if
 * its first halfword still reads 0x6774. For each of four party members it
 * copies fifteen halfwords into the unit record at +0xd8, then reruns
 * Func_8079ae8 and CalcStats. Four more halfwords go into gState at +0x220,
 * +0x222, +0x1f8 and +0x1fa, the magic is cleared, and flag 0x952 is dropped.
 *
 * 51 of 59, with the instruction COUNT exact -- the operations are all right
 * and essentially every register is rotated.
 *
 * BLOCKER: register assignment, downstream of two things I could not reach.
 *
 * 1. THE gState OFFSETS ARE RELATED, AND THE ROM WALKS BETWEEN THEM. It builds
 *    0x220 once as `mov r4, #0x88 / lsl r4, #2`, then reaches 0x222 with
 *    `add r4, #2` and 0x1fa with `sub r4, #0x28`, keeping the running offset in
 *    r4 across all four stores. We emit a fresh `ldr r3, =0x222` and friends.
 *    That is cse's related-value machinery operating on the offsets, and this
 *    notebook's own entry says related_value requires CONST and so never
 *    applies to plain integers -- yet here it plainly does apply, to
 *    gState-relative addresses. Whatever source shape puts the four stores in
 *    that relation, four spellings did not find it.
 *
 * 2. `ble` VERSUS `bls` ON THE OUTER BOUND. The ROM's `ble` is signed, ours is
 *    unsigned, and the recorded rule says the condition code names the
 *    signedness of the counter. It does not here: declaring the counter `int`
 *    rather than `unsigned int` changes NOTHING, 51 either way. gcc knows the
 *    counter is a small non-negative and picks the unsigned form regardless of
 *    the declared type, so this is a case where the signedness tell does not
 *    invert -- worth recording, because the rule is otherwise reliable and it
 *    would be easy to keep sweeping a lever that cannot move.
 *
 * TRIED:
 *   a  `ewram_2001078[0]` tested, then `p = ewram_2001078 + 1`   53 differ
 *   b  ONE pointer: `p = ewram_2001078; if (*p != ...) ; p++`    51  <- best
 *   c  b with the magic read into its own local first            51
 *   d  b with the loop counter declared `int` not `unsigned`     51
 *
 * WHAT WAS WON: the single-pointer form is right and is worth two instructions.
 * The ROM loads the base into r5 and never reloads it -- the same register is
 * the magic's address, then the copy source, then the target of the final
 * zeroing store -- so the source has ONE pointer variable that is read,
 * advanced and reused, not a base plus a derived cursor.
 *
 * The inner copy is `for (n = 14; n >= 0; n--)`, fifteen halfwords: the ROM
 * seeds 14, decrements inside the body and exits on `bge` failing, so the body
 * runs for n = 14 down to 0 inclusive. Counting the iterations off the `sub`
 * position rather than the seed is the thing to get right here.
 */

extern unsigned short ewram_2001078[];
extern unsigned char gState[];
extern unsigned char *GetUnit(unsigned int unit);
extern void Func_8079ae8(unsigned int pc);
extern void CalcStats(unsigned int pc);
extern void ClearFlag(int id);

void Func_807a7a0(void)
{
    unsigned short *p;
    unsigned short *dst;
    unsigned char *g;
    unsigned int i;
    int n;

    p = ewram_2001078;
    if (*p != 0x6774)
        return;
    p++;
    for (i = 0; i <= 3; i++) {
        dst = (unsigned short *)(GetUnit(i) + 0xd8);
        for (n = 14; n >= 0; n--)
            *dst++ = *p++;
        Func_8079ae8(i);
        CalcStats(i);
    }
    g = gState;
    *(unsigned short *)(g + 0x220) = *p;
    p++;
    *(unsigned short *)(g + 0x222) = *p;
    p++;
    *(unsigned short *)(g + 0x1f8) = *p;
    *(unsigned short *)(g + 0x1fa) = p[1];
    ewram_2001078[0] = 0;
    ClearFlag(0x952);
}
