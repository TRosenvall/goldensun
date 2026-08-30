/*
 * CheckEquipmentCritBoost  --  asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_a.s
 *
 * BLOCKER: prologue emission order. 51 lines against 51, TWO differing, and
 * they are the same two instructions swapped:
 *
 *      rom   mov r8, r2   /  mov r1, #0xe
 *      ours  mov r1, #0xe /  mov r8, r2
 *
 * Everything else -- both loops, the register-offset load, the accumulate, the
 * clamp, the epilogue -- is identical.
 *
 * THE ONE THAT MATTERED, and it is reusable: the load is
 * `ldrh r3, [r5, r7]` where r5 walks 0xd8, 0xda, ... and r7 holds the argument.
 * The ROM makes the WALKING OFFSET the base register and the argument the
 * index. Writing the obvious `rec` as `unsigned char *` and indexing it gives
 * `[r7, r5]` -- operands reversed, and every subsequent register shifts. Taking
 * the argument as an `int` and walking a `unsigned char *p = (unsigned char *)0xd8`
 * flips it. That is the documented which-operand-is-the-pointer rule, and this
 * is a clean second instance of it: the C looks wrong (a pointer literal 0xd8)
 * and is what the compiler actually saw.
 *
 * TRIED AND REJECTED, all measured:
 *
 *   * Naming the 0x200 mask as a local `int mask` so it has a birth point
 *     before the counter. This is the obvious fix for an ordering problem and
 *     it is WRONG: naming it makes gcc give it a callee-saved register of its
 *     own, pulling r10 into the frame and moving the argument to r8. 47
 *     differing, and 46 or 50 lines depending on where the assignment sits.
 *     The mask must stay an unnamed loop-invariant that gcc hoists itself.
 *   * Placing that assignment in the ROM's apparent init order
 *     (total, p, mask, i) -- same failure, 47 differing.
 *   * Rewriting the outer `for` as `i = 0xe; do { ... } while (i >= 0);`.
 *     No change at all, still exactly 2 differing. Loop SHAPE is not the lever;
 *     gcc had already proved the loop entered and emitted the rotated form.
 *
 * So the remaining question is narrow: what makes gcc emit a hoisted invariant
 * BEFORE an explicitly initialised counter rather than after. Every spelling
 * tried puts explicit initialisations first and hoisted invariants last. That
 * ordering may not be reachable from source at all, but it has not been proved
 * unreachable, and at 2 of 51 this is one of the closest parks in the tree.
 */
extern unsigned char *GetItemInfo(int id);

int CheckEquipmentCritBoost(int rec)
{
    unsigned char *p;
    unsigned char *q;
    int i;
    int j;
    int total;

    total = 0;
    p = (unsigned char *)0xd8;
    i = 0xe;
    do {
        if (*(unsigned short *)(p + rec) & (0x80 << 2)) {
            q = GetItemInfo(*(unsigned short *)(p + rec)) + 0x18;
            for (j = 3; j >= 0; j--) {
                if (*q == 0x17)
                    total += *(signed char *)(q + 1);
                q += 4;
            }
        }
        p += 2;
        i--;
    } while (i >= 0);
    if (total < 0)
        total = 0;
    return total;
}
