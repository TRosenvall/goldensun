/* Func_8078ad0 (NotifyItemUsed) -- 0x08078ad0,
 * asm/rom_77000/rom_78a8c_c_a.s
 *
 * 15 of 15 lines, THREE differing -- and all three are one register.
 * Candidate at scratch/L78ad0.c.
 *
 *      rom   mov r4, #0x0 ... mov r4, r0 ... mov r0, r4
 *      ours  mov r3, #0x0 ... mov r3, r0 ... mov r0, r3
 *
 * The result variable lands in r4 in the ROM and r3 in ours.  Nothing else
 * differs; the `0x1ff & id` mask with the constant as destination, the table
 * lookup through an asm-labelled extern, and the `i - 1` argument all match.
 *
 * Note r4 does NOT need to survive the call here even though it is set before
 * it: the branch that skips the call is the only path on which the value is
 * read, so gcc is free to use a call-clobbered register.  Under
 * -fcall-used-r4 both r3 and r4 are call-clobbered and REG_ALLOC_ORDER puts r3
 * first, which is why ours picks r3.  For the ROM to pick r4 something else
 * must have held r3, and nothing in this function does.
 *
 * TRIED: declaring the result before the index; naming the masked index as a
 * local; an early-return spelling (8 differing, worse).
 */
