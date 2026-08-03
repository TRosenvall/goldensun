/* OvlFunc_891_20095d4  [ovl_78c76c] and one sibling
 * Source asm: goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_a.s
 *
 * Seventeen against seventeen, diverging at instruction 3 in the set-up for
 * the first call: the ROM interleaves TWO shifted constants and a plain
 * argument as
 *
 *     mov r1, #0xd0 / mov r2, #0xe0 / mov r0, #2 / lsl r1, #16 / lsl r2, #15
 *
 * This is the two-shifted-constants case that gcc DOES produce elsewhere --
 * probe q8 in the session notes emitted exactly that pattern from
 * f3(0xe, 0x102, 0x204). Here it does not, and the plain `mov r0, #2` in the
 * middle is the difference: gcc groups the two shift pairs and puts the
 * unrelated move outside them.
 *
 * So this is the arg-interleave class with a third argument involved, and
 * worth retrying if that class ever falls -- it is not a separate problem.
 */
extern void __Func_8012078(int a, int b, int c, int d);
extern int  OvlFunc_891_2009be8(int a, int b, int c);
extern void OvlFunc_891_200a244(void);

void OvlFunc_891_20095d4(void)
{
    __Func_8012078(2, 0xd0 << 16, 0xe0 << 15, 0);
    if (OvlFunc_891_2009be8(0xa, 0xe, 7))
        OvlFunc_891_200a244();
}
