// fakematch
/* OvlFunc_949_2008260  --  0x02008260
 *
 * Cut out of goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_a_c.s.
 *
 * Two calls: a six-argument one whose last two go on the stack, then a
 * three-argument one taking two shifted constants.
 *
 * PARKED AT 3 OF 20, LENGTH EXACT, on the arg-interleave at the second call:
 *
 *     rom   mov r1,#0xe0 / mov r2,#0xd4 / mov r0,#0x66 / lsl r1,#14 / lsl r2,#17
 *     ours  mov r1,#0xe0 / mov r2,#0xd4 / lsl r1,#14 / lsl r2,#17 / mov r0,#0x66
 *
 * Pinning the three argument registers and assigning them in the ROM's order
 * matches.
 *
 * THE PARK REASONED FROM THE RIGHT LEVER AND THE WRONG UNIVERSE OF LEVERS. Its
 * kin file, src/overlays/rom_7d4af4/ovl_30_c_c_a_a_b.c -- the other half of
 * this same split, and the first function ever matched through this blocker --
 * records that the trigger is WHERE the value is assigned: a literal at the
 * call site comes out contiguous, a named local in the SAME basic block is held
 * in a register, and a named local in a DIFFERENT basic block interleaves. This
 * function is eight instructions of straight-line code with no branch, so there
 * is no second basic block to assign in, and the park concluded the lever could
 * not be applied. That is correct about THAT lever.
 *
 * A pin does not need a second basic block. It names the hard register, so the
 * placement is decided at the assignment rather than by which block the value
 * lives in -- the same boundary recorded across batches 193-197.
 *
 * THE PARK'S OTHER MEASUREMENT IS THE VALUABLE ONE AND IT STILL STANDS: a CALL
 * DOES NOT SUBSTITUTE FOR A BRANCH. Assigning the shifted constants before the
 * FIRST call does not put them in a different basic block, it makes them live
 * ACROSS a call, so gcc keeps them in callee-saved registers, pushes r4 and r5
 * the ROM never pushes, and the function grows three lines to 23. Measured
 * again here and unchanged.
 *
 * KEPT FROM THE PARK, load-bearing: the two stack arguments of __Func_8010704
 * are named locals. The ROM materialises both into registers and then stores
 * both; written as literals gcc builds and stores each in turn, reusing one
 * register. That fix alone took the function from 8 differing to 3.
 */

extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, unsigned int b, unsigned int c);

void OvlFunc_949_2008260(void)
{
    int s1;
    int s2;

    s1 = 3;
    s2 = 0x1a;
    __Func_8010704(3, 0x20, 1, 1, s1, s2);
    {
        register int q0 __asm__("r0");
        register unsigned int q1 __asm__("r1");
        register unsigned int q2 __asm__("r2");
        q1 = 0xe0;
        q2 = 0xd4;
        q0 = 0x66;
        q1 <<= 14;
        q2 <<= 17;
        __Func_808edac(q0, q1, q2);
    }
}
