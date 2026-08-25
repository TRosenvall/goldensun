/* Cluster OvlFunc_946_2009624..OvlFunc_946_2009624 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched -- gcc regenerates the
 * .s at the same path.
 *
 * Matched on the first screen with the stack-arg-pair lever: the two arguments
 * that go on the stack are separate named locals, so both are materialised
 * before either is stored --
 *
 *      mov r3, #0x8 / mov r2, #0x15 / str r3, [sp] / str r2, [sp, #4]
 *
 * Passed as literals, gcc walks one register through both slots instead.
 *
 * The four register arguments happen to be wanted in r0..r3 order here, which
 * is why this one matches where its siblings do not -- see
 * src/non_matching/ovl_7b7f1c/2008870.c and 20088a8.c, which have the same
 * three-call shape but need an argument materialised out of order.
 */
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009624(void)
{
    int e;
    int f;

    __SetFlag(0x8c4);
    e = 8;
    f = 0x15;
    __Func_8010704(0, 0, 1, 1, e, f);
}
