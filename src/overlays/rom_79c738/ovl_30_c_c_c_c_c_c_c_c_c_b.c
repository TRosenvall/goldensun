// fakematch
/* OvlFunc_909_2009958  --  0x02009958
 *
 * Cut out of goldensun/asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c.s.
 * NOTE: the park recorded the source as ovl_30_c_c_c_c_c_c.s, which no longer
 * exists -- that file was split since. Locate a park's asm by grepping for the
 * function name, never by the path in its header.
 *
 * Three calls to the same helper, one per actor slot.
 *
 * PARKED AT 6 OF 18: one difference counted three times, once per call.
 *
 *     rom   mov r1, #0xe0 / mov r0, #0x1 / lsl r1, #0x8 / mov r2, #0x0
 *     ours  mov r1, #0xe0 / mov r0, #0x1 / mov r2, #0x0 / lsl r1, #0x8
 *
 * The shift completing the second argument must happen BEFORE the third
 * argument is materialised. gcc emits the two dependency-free `mov`s adjacent
 * and defers the shift. Pinning the three argument registers and assigning them
 * in the ROM's order places it:
 *
 *     q1 = 0xe0;  q0 = 1;  q1 <<= 8;  q2 = 0;
 *
 * THE PARK'S THREE SPELLINGS WERE ALL VARIATIONS ON ONE THING and it said so
 * honestly -- the shift inline at the call, the shift folded into the
 * initialiser, and a shared zero local hoisted to the top. Every one leaves gcc
 * to decide when the shift happens. The pin is the only construct here that
 * decides it, which is the same boundary recorded across batches 193-196: a
 * spelling proposes, a pin disposes.
 *
 * This is the reachable sub-case of the interleave: the third argument's `mov`
 * has an operation nearby -- the second argument's shift -- whose order the
 * source can set. Compare src/non_matching/ovl_7ebdfc/2008120.c, where the
 * interleaved movs have no consuming operation at all and no pin reaches them.
 *
 * The park kept its candidate body, and that body screens at exactly the 6 of
 * 18 recorded, so the starting point was verifiable before anything changed.
 */

extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2009958(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xe0;
        q0 = 1;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xa0;
        q0 = 2;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 3;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
}
