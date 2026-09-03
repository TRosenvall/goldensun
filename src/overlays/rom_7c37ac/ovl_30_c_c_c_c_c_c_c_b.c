// fakematch
/* OvlFunc_938_2009450  --  0x02009450
 *
 * Cut out of goldensun/asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c.s.
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a.o and
 * asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7c37ac/overlay.ld.
 *
 * Turns slots 0..3 to face angle 0xc000, then waits n frames if n is
 * non-zero.  Per ANNOTATIONS.json, Func_8092adc is TurnSlotToAngle --
 * r0 = slot, r1 = goal facing angle, r2 = frames to wait afterwards -- so
 * the r2 = 0 here means each turn is fired without its own pause and the
 * single trailing wait covers all four.
 *
 * FAKEMATCH, and the plain-C floor is 14 of 30. This is the straight-line
 * constant-hoist blocker: Thumb cannot encode 0xc000, so expand force_reg's it
 * at each of the four call sites -- MEASURED, .00.rtl has four separate
 * `(set (reg) (const_int 49152))` -- and then cse1 deletes three of them and
 * rewrites every `(set r1 regN)` to the survivor. That is `cse.c`'s COST macro
 * at line 509: a pseudo costs 1, a const_int goes through notreg_cost and
 * costs more on Thumb, so at a copy insn CSE always prefers the register.
 *
 * NO FLAG REACHES IT. Swept -fno-rerun-cse-after-loop, -O1,
 * -fno-expensive-optimizations, -fno-gcse, -fno-strength-reduce,
 * -fno-schedule-insns2 and -fno-peephole2: all fourteen, unchanged. That closes
 * the "maybe some CSE flag reaches it" question for the SHIFTED-CONSTANT
 * straight-line class, the same way it is already closed for the pool class.
 *
 * Correction to the existing note, which says the hoist happens at expand:
 * that is true of the POOL-LOAD form only. For the mov+lsl form expand emits
 * four independent sets and the hoist is cse1's.
 *
 * THE IDIOM DOES NOT GO ON EVERY SITE, AND PUTTING IT EVERYWHERE IS WORSE.
 * MEASURED: barrier all four and it is 3 differing; barrier the parameter
 * instead and it is 2; leave site one plain and it is exact. The reason is
 * that the first call already gets the ROM's interleave for free -- the
 * parameter copy `mov r5, r0` is itself the instruction that lands in the
 * `mov r1,#0xc0 / lsl r1,#8` gap. Forcing r0 there pushes the copy to the top.
 * RULE: barrier sites 2..n, never site 1, when a parameter is saved to a
 * callee-saved register.
 *
 * Both halves of the idiom are load-bearing. MEASURED: the
 * `register __asm__("r0")` declaration ALONE is inert -- without the volatile
 * barrier it is fourteen, identical to plain literals. The barrier is what
 * defeats cse1; the register pin is what places `mov r0,#N` in the gap.
 */

extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_938_2009450(int n)
{
    int w1, w2, w3, w4;

    w1 = 0xc0;
    w1 <<= 8;
    __Func_8092adc(0, w1, 0);
    w2 = 0xc0;
    {
        register unsigned int rq __asm__("r0") = 1;
        __asm__ volatile ("" : : "r" (rq));
        w2 <<= 8;
        __Func_8092adc(rq, w2, 0);
    }
    w3 = 0xc0;
    {
        register unsigned int rq __asm__("r0") = 2;
        __asm__ volatile ("" : : "r" (rq));
        w3 <<= 8;
        __Func_8092adc(rq, w3, 0);
    }
    w4 = 0xc0;
    {
        register unsigned int rq __asm__("r0") = 3;
        __asm__ volatile ("" : : "r" (rq));
        w4 <<= 8;
        __Func_8092adc(rq, w4, 0);
    }
    if (n)
        __CutsceneWait(n);
}
