/* Overlay 974: four of the seven message-range dispatch stubs.
 *
 * Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s --
 * it held exactly these four, so nothing further needed splitting. The _a part
 * stays as assembly and is listed before this one in
 * overlays/rom_7fcd20/overlay.ld, so the ROM layout is unchanged.
 *
 * Each stub hands OvlFunc_974_200807c a first message id and the SPAN of a
 * message range. The span is computed at runtime in the ROM -- two pool loads
 * and a subtraction for a value that is constant -- and writing it as
 * `0xd4c - 0xd21` in C makes gcc fold it away.
 *
 * Taking the addresses of absolute symbols prevents the fold: gcc cannot know
 * the difference between two link-time addresses, so it must emit both loads
 * and the subtraction. message.sym defines them and emits no bytes. See
 * ovl_30_a_c_a_c_c_a_b.c for the longer explanation.
 *
 * Note which ids are symbols and which are literals -- it is not arbitrary.
 * The span operands must be symbols or the fold returns. The FIRST argument is
 * a plain literal wherever the ROM loads it independently, and a symbol only
 * in OvlFunc_974_2008180, where the ROM reuses the same register for the
 * argument and the subtraction.
 */

extern void OvlFunc_974_200807c(int firstMessage, int span);

extern int _MSG_c9b;
extern int _MSG_cc6;
extern int _MSG_d21;
extern int _MSG_d4c;

void OvlFunc_974_2008180(void)
{
    OvlFunc_974_200807c((int)&_MSG_d21, (int)&_MSG_d4c - (int)&_MSG_d21);
}

void OvlFunc_974_2008198(void)
{
    OvlFunc_974_200807c(0xd4c, (int)&_MSG_cc6 - (int)&_MSG_c9b);
}

void OvlFunc_974_20081b8(void)
{
    OvlFunc_974_200807c(0xd77, (int)&_MSG_cc6 - (int)&_MSG_c9b);
}

void OvlFunc_974_20081d8(void)
{
    OvlFunc_974_200807c(0xda2, (int)&_MSG_cc6 - (int)&_MSG_c9b);
}
