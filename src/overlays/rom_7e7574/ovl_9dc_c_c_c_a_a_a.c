/* Cluster OvlFunc_959_200cd50..OvlFunc_959_200cd50 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Sets the active message, has an actor speak it, and -- only if the party is
 * carrying item 0xea -- runs the message two ids later through the modal.
 *
 * A SECOND USE OF THE symbol-plus-offset TELL. The ROM parks the base id in a
 * callee-saved register and reaches the second message with `add r0, r5, #2`:
 *
 *     ldr r5, =0x256c / mov r0, r5 / bl __MessageID
 *     ...
 *     add r0, r5, #0x2 / bl __Func_801776c
 *
 * Written with literals, gcc folds 0x256c + 2 at compile time and emits a
 * SECOND pool entry for 0x256e -- one instruction shorter and one pool word
 * longer than the ROM. gcc never folds an offset into a symbol address that
 * way, so the base has to be a symbol: `_MSG_256c` in message.sym, which is
 * where the same shape was recorded for _MSG_25b8. Adding it there costs no
 * bytes and asserts nothing beyond the value.
 *
 * With the symbol the two streams are identical at 20 instructions.
 */

extern unsigned char _MSG_256c[];

extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern int __CheckPartyItem(int item);
extern void __Func_801776c(int id, int b);

void OvlFunc_959_200cd50(void)
{
    __MessageID((int)_MSG_256c);
    __ActorMessage(0x800d, 0);
    if (__CheckPartyItem(0xea) != -1) {
        __Func_801776c((int)(_MSG_256c + 2), 1);
    }
}
