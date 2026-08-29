/* OvlFunc_909_2009958  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c.s
 * Best screen: 6 instructions in disagreeing regions, of 18 (streams same length).
 *
 * BLOCKER CLASS: argument-setup ordering -- the same one documented at length
 * in src/non_matching/ovl_780898/2008dc0.c.
 *
 * Three calls to the same function with the same argument shape, so the ONE
 * difference is counted three times:
 *
 *      rom   mov r1, #0xe0 / mov r0, #0x1 / lsl r1, #0x8 / mov r2, #0x0
 *      ours  mov r1, #0xe0 / mov r0, #0x1 / mov r2, #0x0 / lsl r1, #0x8
 *
 * The shift of the second argument must happen before the third argument is
 * materialised.  gcc emits the two dependency-free `mov`s adjacent and defers
 * the shift.
 *
 * WHAT WAS TRIED, all byte-identical:
 *   1. `a << 8` inline in the argument (kept below).
 *   2. The shift folded into the initialiser, `a = 0xe0 << 8`, so the value is
 *      already complete when the call is written.
 *   3. A single `z = 0` local hoisted to the top of the function and passed as
 *      the third argument to all three calls.
 *
 * Two instructions per call, three calls, no spelling reaches it.
 */
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2009958(void)
{
    int a;
    int b;
    int c;

    a = 0xe0;
    __Func_8092adc(1, a << 8, 0);
    b = 0xa0;
    __Func_8092adc(2, b << 8, 0);
    c = 0x80;
    __Func_8092adc(3, c << 8, 0);
}
