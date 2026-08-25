/* OvlFunc_959_200a468 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * TWO HELD VALUES ALTERNATING INTO ONE STACK SLOT. r6 and r5 are both pushed
 * callee-saved registers and both feed [sp] -- r6 for calls one and three, r5
 * for two and four -- while the [sp, #4] value is rebuilt fresh each time. Two
 * locals assigned at their first use, each passed as the fifth argument of the
 * two calls that want it.
 *
 * That is the batch-49 tell applied twice in one function: a value in a pushed
 * callee-saved register is one the source holds across the calls.
 *
 * Twin of ..._c_b.c (OvlFunc_959_200a410).
 */
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_200a468(void)
{
    int a;
    int b;
    int n;

    n = 0x52;
    a = 0x11;
    __Func_80105d4(8, 0x4d, 1, 2, a, n);
    n = 0x37;
    b = 3;
    __Func_80105d4(8, 0x4d, 1, 2, b, n);
    n = 0x23;
    __Func_8010704(0x12, 0x23, 1, 1, a, n);
    n = 0xa;
    __Func_8010704(2, 0xa, 1, 1, b, n);
}
