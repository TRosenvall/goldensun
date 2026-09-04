// fakematch
/* OvlFunc_938_200940c  --  0x0200940c
 *
 * From goldensun/asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a.s.
 *
 * Member of shape group 0 (branch, {beq, bl, bx, cmp, lsl, mov, pop, push}).
 *
 * Four calls to one helper, then an optional wait. Every call is the ordinary
 * interleave and each one places `mov r0` differently:
 *
 *     mov r1, #0xc0 / mov r5, r0 / lsl r1, #7 / mov r0, #0 / mov r2, #0
 *     mov r1, #0xe0 / mov r0, #1 / lsl r1, #8 / mov r2, #0
 *     mov r1, #0x80 / mov r0, #2 / lsl r1, #6 / mov r2, #0
 *     mov r1, #0xa0 / mov r0, #3 / lsl r1, #8 / mov r2, #0
 *
 * The first is the odd one: gcc's save of the incoming parameter (`mov r5, r0`)
 * occupies the slot the other three give to `mov r0`, so that call's r0 is set
 * AFTER its shift while the other three set it before. Writing the assignments
 * in each call's own ROM order handles all four; the parameter save places
 * itself.
 *
 * The trailing `if (wait != 0) __CutsceneWait(wait);` needs no lever -- the
 * ROM's `cmp r5, #0 / beq` over a single guarded call is what that spelling
 * produces.
 *
 * Near-identical to OvlFunc_938_2009450, already elevated, which is the same
 * four-call shape with a different helper.
 */

extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_938_200940c(int wait)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc0;
        q1 <<= 7;
        q0 = 0;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
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
        q1 = 0x80;
        q0 = 2;
        q1 <<= 6;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xa0;
        q0 = 3;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    if (wait != 0)
        __CutsceneWait(wait);
}
