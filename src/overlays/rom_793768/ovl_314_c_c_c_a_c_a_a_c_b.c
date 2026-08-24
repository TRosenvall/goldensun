/* Cluster OvlFunc_898_2008f3c..OvlFunc_898_2008f3c extracted from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a.o and asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_c.o in
 * goldensun/overlays/rom_793768/overlay.ld.
 *
 * A sound, a table-driven call, and a placement. Fifteen instructions.
 *
 * THE DECLARATION LEVER REORDERS MORE THAN r0. Every previous use of it in this
 * tree has been about where `mov r0` lands. Here it fixes the order two SHIFTS
 * are emitted in:
 *
 *     rom    mov r0, #0xcc / mov r1, #0xa0 / lsl r0, #1 / lsl r1, #1 / mov r2, #5
 *     ours   mov r0, #0xcc / mov r1, #0xa0 / lsl r1, #1 / mov r2, #5 / lsl r0, #1
 *
 * r0 is already first in both. What differs is that gcc interleaves the second
 * shift with the third argument and defers the first shift to the end.
 * Declaring `OvlFunc_898_2008ef4` puts the whole block in the ROM's order.
 *
 * So the lever is better described as fixing the ORDER OF ARGUMENT
 * CONSTRUCTION, not the position of r0 -- r0's position is the case it was
 * first noticed on. Worth trying on any argument-block ordering difference,
 * not only the ones where r0 is misplaced.
 *
 * Naming the two shifted values as locals assigned in the ROM's order does NOT
 * work here, which is the same negative as elsewhere: gcc folds
 * `0xcc << 1` at compile time, so the locals never become live values and the
 * adjacency that fixes the stack-arg-pair class has nothing to hold on to.
 *
 * Its twin src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_c.c differs only in
 * the table label and four constants.
 */
extern void OvlFunc_898_2008ef4(int a, int b, int c);
extern unsigned char L2828[] __asm__(".L2828");
extern void __Func_8010560(void *p, int a, int b);

void OvlFunc_898_2008f3c(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L2828, 0x38, 0x13);
    OvlFunc_898_2008ef4(0xcc << 1, 0xa0 << 1, 5);
}
