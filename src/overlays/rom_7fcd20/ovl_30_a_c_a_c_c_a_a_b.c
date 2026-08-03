/* Overlay 974: the first of the seven message-range dispatch stubs.
 *
 * Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a.s. See
 * ovl_30_a_c_a_c_c_a_b.c for why the span is written as a difference of
 * symbol ADDRESSES: written as a difference of literals, gcc folds it to a
 * single immediate and the two pool loads the ROM has disappear.
 *
 * In these two the first argument is a symbol as well, because the ROM reuses
 * the same register for the argument and for one side of the subtraction.
 */

extern void OvlFunc_974_200807c(int firstMessage, int span);
extern int _MSG_c9b;
extern int _MSG_cc6;

void OvlFunc_974_2008130(void)
{
    OvlFunc_974_200807c((int)&_MSG_c9b, (int)&_MSG_cc6 - (int)&_MSG_c9b);
}
