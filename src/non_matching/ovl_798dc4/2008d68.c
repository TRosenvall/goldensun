/* OvlFunc_903_2008d68  [ovl_798dc4]  --  0x02008d68
 *
 * Source asm: goldensun/asm/overlays/rom_798dc4/ovl_314_c_a_c.s
 *
 * Sets two field-move parameters, ORs bit 3 into a byte at [iwram_3001f30]+0x71c,
 * and runs the move. Twenty-two instructions against twenty-two, and the whole
 * difference is TWO REGISTERS SWAPPED:
 *
 *     rom    ldrb r2, [r5] / mov r3, #0x8 / orr r3, r2
 *     ours   ldrb r3, [r5] / mov r2, #0x8 / orr r3, r2
 *
 * The `orr` is identical on both sides. What differs is which register holds
 * the loaded byte and which holds the constant, and that changes the encoding
 * of the two instructions before it.
 *
 * THIS LOOKS LIKE THE MASK-OPERAND-ORDER RULE AND IS NOT. That rule --
 * `m & s->flags` rather than `s->flags & m`, from batch 12 -- decides which
 * operand ends up as the DESTINATION of the combine. Here the destination is
 * already right; it is the two source registers that are swapped, and the
 * source cannot name them.
 *
 * TRIED, all 2 of 22:
 *   `*p = 8 | *p;`        (mask first, which is what the rule prescribes)
 *   `*p = *p | 8;`
 *   `*p |= 8;`
 *   `v = 8; *p = v | *p;` (the constant as a named local)
 *
 * gcc allocates the byte to r3 and the constant to r2 in every spelling. It is
 * register allocation, not operand order, and nothing in docs/elevation.md
 * reaches it. Recorded so the mask rule is not tried on it a second time --
 * the shapes are easy to confuse and the rule is the first thing anyone would
 * reach for.
 */
extern unsigned int iwram_3001f30;
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int a);
extern void __Func_8097174(void);

void OvlFunc_903_2008d68(void)
{
    unsigned char *p;

    p = (unsigned char *)iwram_3001f30;
    __Func_8096fb0(0x4e, 1);
    __Func_80970f8(2, 0xf);
    p += 0x71c;
    *p = 8 | *p;
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
}
