/* OvlFunc_883_2008e54 -- NOT MATCHING, and this park covers SIX functions.
 *
 * tools/prologue_families.py finds six functions sharing all sixteen of their
 * instructions up to the constants:
 *
 *   OvlFunc_883_2008e54  asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *   OvlFunc_883_2008e84  asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *   OvlFunc_883_2008f5c  asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *   OvlFunc_883_2008f8c  asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a.s
 *   OvlFunc_884_200881c  asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_a.s
 *   OvlFunc_884_20088ac  asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_c.s
 *
 * Best screen: 16 instructions against the ROM's 16, 3 differing. Solving this
 * one elevates six.
 *
 * BLOCKER CLASS: arg-interleave in a STRAIGHT-LINE function.
 *
 *     rom    mov r1, #0xcb / mov r0, #0 / lsl r1, #1 / ldr r2, =0x2d7
 *     ours   mov r1, #0xcb / lsl r1, #1 / ldr r2, =0x2d7 / mov r0, #0
 *
 * The ROM wedges `mov r0, #0` into the `mov`/`lsl` pair and issues the pool
 * load last. Both halves of that are shapes the BASIC-BLOCK LEVER reaches --
 * and the lever needs a branch to put between the assignment and the use, and
 * this function has none. Sixteen instructions, four calls, no control flow at
 * all. docs/elevation.md states that limit explicitly ("a straight-line
 * function cannot use this") and this is a clean instance of it.
 *
 * MEASURED, all 3 of 16 except where noted:
 *   __Func_809218c declared `int` (the return-type lever)
 *   __Func_8010560 declared `int`                    -- 6 of 16, worse
 *   both shifted argument and pool constant as locals at the top of the
 *     function                                       -- 19 lines, much worse
 *   the shifted argument as a local in a bare block immediately before the call
 *
 * The batch-106 argument-order table says why the return-type lever cannot
 * help: it moves r0 between first and last, and the ROM wants r0 in the
 * MIDDLE of a three-argument call. Nothing in that table produces that.
 *
 * NEXT: this is the one shape a family this size is worth revisiting for. If a
 * future batch finds any construct that puts r0 in the middle without a
 * branch, six functions come with it.
 */
extern unsigned char L755a[] __asm__(".L755a");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008e54(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L755a, 0x36, 0x20);
    __Func_809218c(0, 0xcb << 1, 0x2d7);
    __Func_8091e9c(5);
}
