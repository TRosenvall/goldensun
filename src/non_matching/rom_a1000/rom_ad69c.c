/* Func_80ad69c -- NOT MATCHING. 5 of 25, and ours is one instruction short.
 *
 * Source asm: goldensun/asm/rom_a1000/rom_ad274_c_c.s
 *
 * Blocker: THE ROM HOLDS THE OFFSET AND RECOMPUTES THE ADDRESS; gcc holds the
 * address.
 *
 *     rom    ldr r1, =0x219 ... add r3, r2, r1 / ldrb r3, [r3]   (the guard)
 *                            ... add r7, r2, r1                  (the loop)
 *     ours   ldr r3, =0x219 / add r7, r2, r3 / ldrb r3, [r7]     (both)
 *
 * The count byte is read twice -- once before the loop and once as its
 * condition -- and the ROM computes `p + 0x219` separately for each while
 * keeping 0x219 itself in a register. gcc computes the address once and reuses
 * it, which is one instruction fewer.
 *
 * TRIED: naming the offset and writing `*(p + off)` at both sites, so the
 * address is a fresh expression each time. That restores the LENGTH -- 25
 * against 25 -- but the count goes to 6, because the offset then occupies a
 * different register than the ROM's and the whole prologue shifts.
 *
 * That is the "one local per independent operation" rule (batch 57) pointing at
 * an answer it cannot deliver: the reference does say the two address
 * computations are independent, and saying so in the source does not reproduce
 * the allocation.
 *
 * The `ldmia r5!, {r0}` is a single-register load with writeback -- `*q++` --
 * and that part is right. It is the load twin of the `stmia` loop parked in
 * src/non_matching/rom_15000/rom_198dc.c, and this is the tree's first of them.
 */
extern unsigned char *iwram_3001f2c;
extern void _Sprite_SetAnim(void *s, int anim);

void Func_80ad69c(void)
{
    unsigned char *p;
    unsigned char *cnt;
    void **q;
    int i;

    p = iwram_3001f2c;
    i = 0;
    if (i >= *(p + 0x219))
        return;
    cnt = p + 0x219;
    q = (void **)(p + (0x8a << 1));
    do {
        _Sprite_SetAnim(*q++, 1);
        i++;
    } while (i < *cnt);
}
