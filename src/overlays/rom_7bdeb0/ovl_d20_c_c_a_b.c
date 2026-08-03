/* Overlay 934: apply a map edit at row 0xF.
 *
 * Split out of asm/overlays/rom_7bdeb0/ovl_d20_c_c_a.s. One of a pair
 * differing only in the row -- 0xF and 0x11.
 *
 * Note the two stack arguments are the SAME value here, so the ROM reuses one
 * register for both stores and gcc does too. Where a pair of stack arguments
 * differs, the ROM builds them into separate registers first and gcc does not
 * -- that is the "stack-arg-pair" blocker, and it gates fifty functions.
 */

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);

void OvlFunc_934_2008dcc(void)
{
    __Func_80105d4(0x10, 0xf, 1, 1, 0xf, 0xf);
}
