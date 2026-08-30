/*
 * Func_808e0b0  (SetObjectVisibility)  --  asm/rom_8a000/rom_8d9a4_a_c_a_a_a_a.s
 *
 * BLOCKER: register-role swap in a loop preheader. 52 lines against 52, THREE
 * differing, and they are one register choice:
 *
 *      rom   mov r3, r12 / add r3, #0x27 / ldrb r3, [r3]
 *      ours  mov r2, r12 / add r2, #0x27 / ldrb r3, [r2]
 *
 * The ROM computes the address of the count field into r3 and loads back into
 * the SAME register; we compute it in r2 and load into r3. Nothing else in the
 * function differs -- the draw-kind mask, the table lookup, the three-condition
 * filter in the loop, and the epilogue all match.
 *
 * THIS IS THE SAME BLOCKER AS Func_8096b88, in the same shape, on the same
 * field offset (+0x27 count, +0x28 actor array, +0x25 flag). They are a family
 * and should be re-attacked together; whatever spelling makes gcc collapse the
 * address temp into the destination register will very likely close both.
 *
 * SETTLED, and it is the transferable half:
 *
 *   The count is read TWICE in the source, and `list` is assigned BEFORE `n`.
 *   Both matter and they were measured separately here:
 *
 *     n first, then list   -- 5 differing
 *     list first, then n   -- 3 differing   <- this file
 *
 *   Note this is the OPPOSITE of what the same swap did on Func_8096b88, where
 *   putting list first took 7 differing to 8. Same family, same two statements,
 *   opposite sign. So the ordering is not a rule that transfers even between
 *   near-identical functions; it has to be measured per function.
 *
 * TRIED AND REJECTED, all measured:
 *
 *   * Single read (`n = o[0x27]; if (n != 0)`) -- 51 lines, one SHORT, 26
 *     differing. gcc keeps the loaded byte in the counter register and never
 *     emits the ROM's copy.
 *   * `unsigned char n` instead of `int n` -- 55 lines, 17 differing. Much
 *     worse; the narrower type adds truncation.
 *   * Naming the count's ADDRESS in its own local (`cp = o + 0x27;` then
 *     `*cp`), on the theory that naming the address would make gcc reuse one
 *     register for it. NO CHANGE AT ALL -- still exactly 3 differing. This was
 *     the most promising idea and it did nothing.
 */
extern unsigned int iwram_3001e40;
extern unsigned char L9e6b8[] __asm__(".L9e6b8");

void Func_808e0b0(unsigned char *e, int visible)
{
    unsigned char *o;
    unsigned char **list;
    unsigned char *q;
    int v;
    int n;

    if ((*(unsigned char *)(e + 0x54) & 0xf) != 1)
        return;
    o = *(unsigned char **)(e + 0x50);
    v = visible - 1;
    if (visible == 0)
        v = L9e6b8[(iwram_3001e40 >> 1) & 7];
    if (o[0x27] != 0) {
        list = (unsigned char **)(o + 0x28);
        n = o[0x27];
        do {
            q = *list++;
            if (q != 0 && *(int *)(q + 0x10) != 0 && q[5] != 0xf)
                q[5] = v;
            n--;
        } while (n != 0);
    }
    o[0x25] = 1;
}
