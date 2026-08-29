/* Overlay 974: one of a family of seven near-identical dispatch stubs.
 *
 * Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a.s; the _a and _c
 * parts stay as assembly and are listed around this one in
 * overlays/rom_7fcd20/overlay.ld, so the ROM layout is unchanged.
 *
 * WHY THE SECOND ARGUMENT IS WRITTEN THIS WAY
 *
 * The ROM computes the message-id span AT RUNTIME:
 *
 *     ldr r3, =0xc9b / ldr r1, =0xcc6 / sub r1, r3
 *
 * Two pool words and a subtraction, for a value that is 0x2B and constant.
 * Written as `0xcc6 - 0xc9b` in C, gcc folds it to `mov r1, #0x2b` and one
 * pool word disappears -- which is why this function sat parked.
 *
 * The fold is impossible when the operands are the addresses of ABSOLUTE
 * SYMBOLS rather than literals: gcc cannot know the difference between two
 * link-time addresses, so it must emit both loads and the subtraction. The
 * message ids already have that treatment elsewhere in the tree -- the
 * matched src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_b.c writes
 * `__MessageID((int) (&_MSG_1ea0))` -- and message.sym is the linker fragment
 * that defines them. `_MSG_c9b` and `_MSG_cc6` are added there; a linker
 * symbol definition emits no bytes.
 *
 * That is exactly what the parked note on the sibling functions asked for:
 * "to stop the fold, X and Y must be SYMBOLS, not literals; identify the two
 * symbols per call site."
 *
 * The remaining six siblings take the same treatment. Each needs its own pair
 * added to message.sym: 0xd21/0xd4c for OvlFunc_974_2008180, and the rest
 * reuse 0xc9b/0xcc6 with a different first argument.
 */

extern void OvlFunc_974_200807c(int firstMessage, int span);

/* The two ends of the message range this stub selects. Their VALUES are the
 * ids; only their addresses are ever taken.
 */
extern int _MSG_c9b;
extern int _MSG_cc6;

void OvlFunc_974_2008160(void)
{
    OvlFunc_974_200807c(0xcf1, (int)&_MSG_cc6 - (int)&_MSG_c9b);
}
