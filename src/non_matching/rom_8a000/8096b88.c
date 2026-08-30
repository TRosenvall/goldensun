/*
 * Func_8096b88  (SetEntityVisible)  --  asm/rom_8a000/rom_944ec_a_c_c_a_c.s
 *
 * BLOCKER: register-role swap in a loop preheader. 49 lines against 49 -- the
 * instruction COUNT is right and the whole body matches; 7 differ, all of them
 * downstream of one register choice.
 *
 *      rom   mov r3, r8 / add r3, #0x27 / ldrb r3, [r3]
 *      ours  mov r2, r8 / add r2, #0x27 / ldrb r3, [r2]
 *
 * The ROM computes the address of the count field into r3 and loads back into
 * the same register; we use r2 for the address. From there the preheader
 * orders differently (`mov r10, r1` against `mov r6, r3`).
 *
 * SETTLED, and it bought the missing instruction:
 *
 *   The count is read TWICE in the source, not once. Writing
 *   `n = o[0x27]; if (n != 0) {...}` gives 48 lines -- one short -- because
 *   gcc keeps the loaded byte in the loop-counter register directly. Writing
 *   `if (o[0x27] != 0) { n = o[0x27]; ...}` produces the ROM's extra
 *   `mov r6, r3`: gcc CSEs the two loads to one but still copies the value into
 *   a callee-saved register for the loop. The redundant-looking re-read is what
 *   the original source had.
 *
 * TRIED AND REJECTED, measured:
 *
 *   * Initialising `list` before `n` inside the guarded block, to match the
 *     ROM's r7-then-r6 birth order. Made it worse, 7 differing to 8.
 *
 * ALSO SETTLED, so nobody re-derives it: the `and r3, r2` that tests bit 0 of
 * o[0x1d] reuses r2 -- the byte loaded from +0x54 -- as the constant 1, because
 * control flow has already proved it equals 1. That falls out of a plain
 * `o[0x1d] & 1`; no spelling is needed for it, and the first 16 instructions
 * match because of it.
 *
 * The second argument in the annotation (r1 = visibility) is never read by the
 * function; the C takes one parameter and that does not affect codegen.
 */
extern unsigned int iwram_3001e40;

void Func_8096b88(unsigned char *e)
{
    unsigned char *o;
    unsigned char **list;
    unsigned char *q;
    int n;

    if (*(unsigned char *)(e + 0x54) != 1)
        return;
    o = *(unsigned char **)(e + 0x50);
    if (o == 0)
        return;
    if (o[0x1d] & 1)
        return;
    if (o[0x27] != 0) {
        n = o[0x27];
        list = (unsigned char **)(o + 0x28);
        do {
            q = *list++;
            q[5] = iwram_3001e40 % 6;
            n--;
        } while (n != 0);
    }
    o[0x25] = 1;
}
