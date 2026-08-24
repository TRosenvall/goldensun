/* Cluster OvlFunc_935_2008410..OvlFunc_935_2008410 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the
 * .o keeps its name and its slot in the overlay's linker script is unchanged.
 *
 * Three map-rect edits in a row. Two six-argument calls to __Func_80105d4 and
 * one to __Func_8010704, all with the last two arguments on the stack.
 *
 * THE STACK-ARG-PAIR LEVER, ALL THREE OF ITS CASES IN ONE FUNCTION, which is
 * why this one is worth reading before attempting another member of the class:
 *
 *   call 1  two DIFFERENT literals (0x50, 9) -- needs the lever. Written as
 *           literals at the call site gcc reuses one register and interleaves
 *           each build with its own store:
 *
 *             rom    mov r3,#0x50 / mov r2,#9 / str r3,[sp] / str r2,[sp,#4]
 *             ours   mov r3,#0x50 / str r3,[sp] / mov r3,#9 / str r3,[sp,#4]
 *
 *           Naming both, in the order the ROM stores them, fixes it.
 *   calls   the low stack slot is 0x11 in BOTH, so it is named once and used
 *   2 and 3 twice -- that is the "shared value" case, and here the sharing runs
 *           ACROSS A CALL rather than between a register and a stack argument.
 *           gcc keeps it in r5 exactly as the ROM does, unprompted.
 *
 * Note call 3 passes 0x11 in r0 as well, and that one is written as a literal:
 * the ROM materialises it fresh with `mov r0, #0x11` rather than copying r5,
 * so naming it would have produced a `mov r0, r5` that is not there.
 */

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_935_2008410(void)
{
    int s = 0x11;
    int e = 0x50, f = 9;

    __Func_80105d4(0x5a, 9, 2, 3, e, f);
    __Func_80105d4(0x1b, 0xa, 1, 2, s, 0xa);
    __Func_8010704(0x11, 0xa, 1, 1, s, 0xb);
}
