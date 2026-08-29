/* Cluster OvlFunc_952_2008564..OvlFunc_952_2008564 extracted from goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d768c/ovl_30_c_a_a_c_b.o and asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.o in
 * goldensun/overlays/rom_7d768c/overlay.ld.
 *
 * A three-message prompt: set the base message, have the actor turn, ask, and
 * then show base+1 or base+2 depending on the answer.
 *
 * TWO LEVERS, both already in the tree and both needed here.
 *
 * 1. THE BASE IS A SYMBOL. The ROM parks it in r5 and reaches the other two
 *    with `add r0, r5, #1` / `add r0, r5, #2`. Written as a literal, gcc
 *    folds the additions and emits three separate pool entries. gcc never
 *    folds an offset into a symbol address, so `_MSG_22ab` goes in message.sym --
 *    exactly the case its "base of a three-message prompt" comment describes.
 * 2. __Func_8092c40 IS NOT DECLARED. Declared, gcc builds its arguments
 *    `mov r0, r6 / mov r1, #0`; the ROM has them the other way round. This is
 *    the declaration lever in its subtractive form -- REMOVING a prototype is
 *    as much a lever as adding one, and it is the form the sibling
 *    src/overlays/rom_7ebdfc/ovl_30_c_c_a_c_b.c uses for the same callee.
 */
extern int _MSG_22ab;

extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int slot, int b);

void OvlFunc_952_2008564(int slot)
{
    int base = (int)(&_MSG_22ab);

    __MessageID(base);
    __Func_8092c40(slot, 0);
    if (!__Func_8091c7c(0, 0)) {
        __MessageID(base + 1);
    } else {
        __MessageID(base + 2);
    }
    __ActorMessage(slot, 0);
}
