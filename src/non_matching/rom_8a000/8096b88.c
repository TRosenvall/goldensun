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
  *
 * VOLATILE: TRIED, NO CHANGE. Batch-142-era work found that gKeyHeld and
 * iwram_3001e40 are declared volatile in some translation units and not
 * others, and that the difference unlocked OvlFunc_933_2008344 outright and
 * halved Func_80b86ec. This function was re-screened with every scalar global
 * marked volatile and the output is BYTE-IDENTICAL, so the missing re-reads
 * are not its problem. Do not try it again.
 *
 * FAMILY UPDATE: Func_808e0b0, the other member of this family, is now
 * ELEVATED. The lever that closed it -- struct types for the object and entry,
 * plus the guard written as `i = 0; if (i < o->f27)` rather than
 * `if (o->f27 != 0)`, with the walking-pointer do-while inside -- was found by
 * grepping GENERATED asm for the same address-temp shape and reading the C that
 * produced it (src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_b.c).
 *
 * IT DOES NOT TRANSFER HERE. Measured: struct types with the `i < o->f27`
 * guard, 12 differing; struct types with `!= 0`, 11; with `0 < o->f27`, 11;
 * with the two preheader assignments swapped, 11; without the unused counter,
 * 11. All are WORSE than the plain-pointer version kept below, which is 7.
 * This function has an extra guard (`o->flags & 1`) ahead of the loop that the
 * elevated one does not, and the register pressure differs.
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
