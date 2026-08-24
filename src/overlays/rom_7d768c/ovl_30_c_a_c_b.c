/* Cluster OvlFunc_952_200bfc4..OvlFunc_952_200bfc4 extracted from goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d768c/ovl_30_c_a_c_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7d768c/overlay.ld.
 *
 * A three-message prompt, the variant that repeats the actor line in BOTH arms
 * rather than joining first. See src/overlays/rom_7d768c/ovl_30_c_a_a_a_b.c for
 * the shared levers: the base id is the symbol `_MSG_22a3` (the ROM reaches the
 * other two with `add r0, r6, #1` and `#2`, which gcc emits only for a symbol
 * address), and `__Func_8092c40` is deliberately left UNDECLARED so its two
 * arguments come out in the ROM's order.
 *
 * Worth noting against the sibling: there the base lands in r5 and the slot in
 * r6, here it is the other way round, and the C is the same shape either way.
 * The allocation follows which value the ROM's compiler happened to see first,
 * not anything the source controls -- so a register swap between two otherwise
 * identical functions is not evidence that the sources differ.
 */
extern int _MSG_22a3;

extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int slot, int b);

void OvlFunc_952_200bfc4(int slot)
{
    int base = (int)(&_MSG_22a3);

    __MessageID(base);
    __Func_8092c40(slot, 0);
    if (!__Func_8091c7c(0, 0)) {
        __MessageID(base + 1);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(base + 2);
        __ActorMessage(slot, 0);
    }
}
