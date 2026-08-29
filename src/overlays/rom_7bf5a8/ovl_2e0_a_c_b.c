/* Cluster OvlFunc_935_2008398..OvlFunc_935_2008398 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bf5a8/ovl_2e0_a_c_a.o and asm/overlays/rom_7bf5a8/ovl_2e0_a_c_c.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld.
 *
 * The twin of src/overlays/rom_7bf5a8/ovl_2e0_c_a_a.c, same overlay, twenty-odd
 * .o slots apart, differing only in its nine constants. Read that file first:
 * it carries all three cases of the stack-arg-pair lever, and this one uses the
 * same two --
 *
 *   call 1  two DIFFERENT stack literals (0x50, 0x32), so both are named,
 *           assigned immediately before the call, in the order the ROM stores
 *           them;
 *   calls   the [sp] value is 0x10 in both, named once and used twice, with the
 *   2 and 3 sharing running ACROSS a call. gcc parks it in r5 unprompted.
 *
 * And the same exception: call 3 passes 0x10 in r0 as well, written as a
 * literal, because the ROM materialises it fresh with `mov r0, #0x10` rather
 * than copying r5.
 */

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_935_2008398(void)
{
    int s = 0x10;
    int e = 0x50, f = 0x32;

    __Func_80105d4(0x57, 0x32, 2, 4, e, f);
    __Func_80105d4(0x17, 0x34, 1, 2, s, 0x34);
    __Func_8010704(0x10, 0x34, 1, 1, s, 0x35);
}
