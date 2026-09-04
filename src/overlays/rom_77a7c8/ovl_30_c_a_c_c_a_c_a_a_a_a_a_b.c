// fakematch
/* OvlFunc_881_200955c  --  0x0200955c
 *
 * Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a.s.
 *
 * TWELVE CALLS TO ONE HELPER, each followed by __WaitFrames(4), stepping a
 * value through a fixed sequence. No branches.
 *
 * TWINS: OvlFunc_881_2009680 in the same .s is the same twelve-call sequence with a
 * different base constant (0xd9 against 0xdf) and a different set of
 * pooled values. Both were generated from one description of the call list and
 * both matched on the first screen.
 *
 * WHY SOME CONSTANTS ARE POOLED AND SOME ARE NOT, which is the thing to read
 * off the listing rather than assume. Four of the twelve build their third
 * argument with `mov r2, #0xdf / lsl r2, #19`; the other eight load it from
 * the pool. 0xdf << 19 is a shifted byte and Thumb can build it in two
 * instructions, while the others -- 0x6fc0000 and its neighbours -- are not, so
 * they cost a pool entry. Writing the literals plainly gets both cases right
 * with no help; the pins are only for the ORDER of the four argument registers.
 *
 * THE LAST CALL LOADS r0 EARLY. Eleven of the twelve fill r0 last, after the
 * negation, the shift and r3; the twelfth puts `ldr r0` between the two movs
 * and the neg/lsl pair. One call site out of twelve, in an otherwise identical
 * sequence, and it is the reason the whole call list was transcribed from the
 * listing instead of written as a loop over a table.
 *
 * FOUND BY LOOKING FOR FUNCTIONS WITH NO PARK. Everything at 60 instructions
 * or fewer in this shape group is parked, and those parks are concentrated in
 * the allocation-order class the pin does not reach. Widening the survey to 120
 * instructions and filtering for unparked functions turned these up; at 103
 * instructions each they are far larger than anything in the parked band and
 * far easier.
 */

extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);

void OvlFunc_881_200955c(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q2 = 0xdf;
        q1 = -q1;
        q2 <<= 19;
        q3 = 1;
        q0 = 0x160c0000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6fc0000;
        q3 = 1;
        q0 = 0x16040000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6f40000;
        q3 = 1;
        q0 = 0x160c0000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6fc0000;
        q3 = 1;
        q0 = 0x160c0000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6f40000;
        q3 = 1;
        q0 = 0x16040000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q2 = 0xdf;
        q1 = -q1;
        q2 <<= 19;
        q3 = 1;
        q0 = 0x16080000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q2 = 0xdf;
        q1 = -q1;
        q2 <<= 19;
        q3 = 1;
        q0 = 0x160a0000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6fa0000;
        q3 = 1;
        q0 = 0x16060000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6f60000;
        q3 = 1;
        q0 = 0x160a0000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6fa0000;
        q3 = 1;
        q0 = 0x160a0000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q1 = -q1;
        q2 = 0x6f60000;
        q3 = 1;
        q0 = 0x16060000;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 1;
        q2 = 0xdf;
        q0 = 0x16080000;
        q1 = -q1;
        q2 <<= 19;
        q3 = 1;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __WaitFrames(4);
}
