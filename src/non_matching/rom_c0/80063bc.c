/* Func_80063bc  --  0x080063bc, asm/rom_c0/rom_5cf8_a_a_c_a.s
 *
 * BLOCKER CLASS: register allocation (REG_ALLOC_ORDER).
 * Status: 30 lines against the ROM's 29. Every instruction matches except one
 * extra register move at entry. Semantics are settled; only the allocation is.
 *
 * WHAT IT DOES
 * Posts a request into the block starting at ewram_2002080, but only when the
 * slot is empty -- otherwise it returns -1 and touches nothing. The body runs
 * with interrupts disabled and restores the saved IME on the way out.
 *
 * THE IME STORE IS NOT A TRANSCRIPTION SLIP. The ROM's disable is
 *
 *      ldr  r2, =0x4000208
 *      ldrh r1, [r2]
 *      strh r2, [r2]        <-- stores the register's own address
 *
 * which writes 0x0208 into REG_IME. Only bit 0 of that register is live and
 * 0x208 has it clear, so this disables interrupts exactly like a plain zero
 * would, one instruction cheaper -- no `mov r3, #0` is needed because the
 * address is already in hand. `*ime = (unsigned int)ime;` reproduces it
 * exactly; writing `REG_IME = 0;` does not, and writing
 * `REG_IME = REG_ADDR_IME;` does not either -- gcc materialises the truncated
 * constant from a fresh literal instead of reusing the address register.
 *
 * THE DIFF
 *      rom   ldr r5, =0x2002080 / ldr r4, [r5] / mov r6, r1 / ldr r7, =0x2002220
 *      ours  ldr r6, =0x2002080 / ldr r4, [r6] / mov r5, r0 / ldr r7, =0x2002220
 *                                                          / mov r0, r1
 *
 * The ROM leaves argument 0 in r0 for its whole life and spills argument 1 to a
 * callee-saved register. We do the opposite: argument 0 goes to r5 and argument
 * 1 is moved into r0. Both are forced out of r1 by the IME read landing there;
 * the question is only which one keeps r0, and gcc-2.96 gives r0 to the value
 * with the EARLIER first use -- argument 1, stored at ewram_2002008 -- while the
 * ROM gives it to the later one.
 *
 * WHAT WAS TRIED (six variants, all identical in this respect)
 *   - with and without a named `slot` pointer for &ewram_2002080
 *   - with and without a named `flags` pointer for ewram_2002220
 *   - a copy `bb = b` placed to match the ROM's pseudo-creation order, which
 *     is readable from its allocation (r5 = slot, r4 = cur, r6 = b, r7 = flags)
 *   - `save` as int, unsigned short and vu16
 * Every one produced the same two moves. The store order cannot be permuted to
 * fix it, because the store order is what already matches.
 *
 *
 * ITS SIBLING Func_8006408 (0x08006408) IS THE SAME FUNCTION over a different
 * pair of globals and is blocked the same way, so it is not parked separately.
 * One detail there is worth carrying: where this one stores the already-zero
 * `cur` register into ewram_20023a4, the sibling loads the zero FROM A POOL --
 *
 *      ldr r0, .L6440   @ 0
 *
 * -- with a zero already sitting in r4 one instruction away. gcc never pools a
 * value it can build with `mov #imm8`, and never pools one it already has in a
 * register, so that operand was a SYMBOL whose value is zero. The only
 * zero-valued symbols currently defined are _AREA_00 and gMaxLines, and neither
 * fits a save-request block, so the namespace is unidentified and it has NOT
 * been named. The consumer that would settle it is whatever reads
 * ewram_20023a4.
 *
 * This is the REG_ALLOC_ORDER divergence documented in docs/elevation.md:
 * arm.h:989 lists {3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ...}, gcc reaches for
 * caller-saved registers first, and there is no Thumb override. Not reachable
 * from C.
 */

typedef volatile unsigned short vu16;

extern int ewram_2002080;
extern unsigned char ewram_2002220[];
extern unsigned short ewram_2002008;
extern unsigned char ewram_20023a4;

int Func_80063bc(int a, int b)
{
    unsigned char *flags;
    vu16 *ime;
    int cur;
    int save;

    cur = ewram_2002080;
    flags = ewram_2002220;
    if (cur != 0)
        return -1;
    ime = (vu16 *)0x4000208;
    save = *ime;
    *ime = (unsigned int)ime;   /* 0x0208: bit 0 clear, so interrupts off */
    flags[1] = 0x80;
    ewram_2002008 = b;
    ewram_20023a4 = cur;        /* cur is zero here; the ROM reuses the register */
    ewram_2002080 = a;
    flags[0] = 1;
    *ime = save;
    return 0;
}
