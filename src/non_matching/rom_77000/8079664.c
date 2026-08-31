/* Func_8079664 -- asm/rom_77000/rom_79460_c_a_a_a.s
 *
 * BLOCKER: REGISTER ROTATION. 26 of 46, three lines short.
 *
 * Removes a party member: find the id in the byte array at gState+0x1f8,
 * shift the remainder down one, return GetPartySize().
 *
 * THE CORE IS A TWO-REGISTER SWAP that nothing reached:
 *
 *     rom    id -> r5   size -> r6
 *     ours   id -> r6   size -> r5
 *
 * and it propagates through every compare in the search loop. The search loop
 * itself is exact -- gcc rotates `for (i = 0; i < size; i++)` into the ROM's
 * shape (guard, element test, then increment/guard/advance/test) with no help.
 *
 * MEASURED:
 *   baseline                                          44 lines, 27 differ
 *   `id` copied to an explicit local assigned FIRST,
 *     and `size - 1` named once and used twice        43 lines, 26 differ
 *   + the tail's address built at runtime as
 *     `(i + (int)gState) + (0xfc << 1)`               43 lines, 32 differ
 *   the same without the explicit id local            43 lines, 32 differ
 *   the same with size declared before id             43 lines, 32 differ
 *
 * THE THIRD LINE IS THE INSTRUCTIVE FAILURE. The tail's residue looked like
 * the documented gState lever: ours folded to `ldr r3, =gState+504` where the
 * ROM builds the offset at runtime (`mov r4, #0xfc / lsl r4, #1 / add`).
 * Writing the address as an explicit runtime sum to force that made the
 * function SIX WORSE. The lever is real -- the SAME function's search loop
 * gets the runtime construction with no help at all, `ldr r0, =gState /
 * mov r2, #0xfc / lsl r2, #1 / ldrb r3, [r0, r2]` -- so what decides it is not
 * how the address is spelled at that one site.
 *
 * Recorded because "our version folded the gState offset, so spell the offset
 * out" is the obvious next move and it is wrong here. The fold is downstream
 * of the register assignment, not a cause of it.
 *
 * The r5/r6 swap is the same allocator-preference class as
 * src/non_matching/rom_b5000/80c0228.c and rom_9000/800f9f4.c. Both live
 * values are function-long, both are call-crossing, and neither an explicit
 * assignment position nor declaration order moved them -- which is a bound on
 * the assignment-position lever recorded in docs/elevation.md: it decided an
 * r0/r1 pair on Func_80d66cc, and it does not decide this callee-saved pair.
 */
extern unsigned char gState[];
extern int GetPartySize(void);
extern void ClearFlag(int id);

int Func_8079664(int id)
{
    int a;
    int size;
    int i;
    int last;
    int n;
    unsigned char *p;

    a = id;
    size = GetPartySize();
    ClearFlag(a);
    for (i = 0; i < size; i++) {
        if (gState[0x1f8 + i] == a)
            break;
    }
    last = size - 1;
    if (i < last) {
        n = last - i;
        p = &gState[0x1f8 + i];
        do {
            *p = p[1];
            n--;
            p++;
        } while (n != 0);
    }
    return GetPartySize();
}
