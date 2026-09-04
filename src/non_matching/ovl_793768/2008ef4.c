/* OvlFunc_898_2008ef4  --  0x02008ef4  [asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.s]
 *
 * NOT MATCHING. 4 of 30, LENGTH EXACT, and the whole residue is WHICH
 * CALLEE-SAVED REGISTER EACH PARAMETER LANDS IN.
 *
 * The function takes three arguments, holds all three across a call, and the
 * ROM parks them like this:
 *
 *     rom    mov r6, r1 / mov r8, r2 / ... / mov r5, r0
 *     ours   mov r5, r1 / mov r8, r2 / ... / mov r6, r0
 *
 * r8 agrees. r5 and r6 are exchanged: the ROM puts the SECOND parameter in r6
 * and the first in r5, gcc does the opposite. Everything downstream follows --
 * `mov r2, r6 / mov r0, #0 / mov r1, r5` against our `mov r2, r5 ... mov r1, r6`
 * -- so two swapped movs at the top cost four differing lines.
 *
 * The C is not in doubt. The call is __Func_809218c(0, a, b) and the tail is
 * __Func_8091e9c(c); the near-twin OvlFunc_899_20099a4, elevated this batch,
 * shares the entire second half of this function and matches exactly.
 *
 * MEASURED, all four byte-identical at 4 of 30:
 *
 *   1. as written, parameters used directly at the call
 *   2. both held values PINNED, `register int rb __asm__("r6")` and
 *      `register int ra __asm__("r5")`, assigned in the ROM's order
 *   3. the argument assignments reordered at the call site
 *   4. the two held values named as ordinary `int` locals, b first
 *
 * THE PIN IS INERT HERE AND THAT IS THE POINT. Every other use of the pin in
 * batches 193-202 places a value into a CALL-CLOBBERED register at a call site.
 * This asks for something different: which CALLEE-SAVED register a value lives
 * in for the whole function. gcc coalesces the pinned local with the incoming
 * parameter and then allocates as it pleases, so naming r5 and r6 changes
 * nothing.
 *
 * That is consistent with the boundary already recorded in
 * src/non_matching/rom_c0/rom_64b8.c: a pin decides which register holds a
 * value and where its own write sits among other pinned writes. It does not
 * override the allocator's choice for a value that is not being written at the
 * point the pin names.
 *
 * AN EXACT TWIN EXISTS AND IS ALSO PARKED BY THIS FILE.
 * OvlFunc_901_2008a80, in asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c_c.s,
 * is INSTRUCTION-FOR-INSTRUCTION IDENTICAL to this function -- same prologue,
 * same three held parameters, same calls, same tail. It was read during batch
 * 203 and deliberately NOT attempted: the same C produces the same four
 * differing lines, and screening it again would only re-measure this wall.
 * Solving either one solves both.
 *
 * NEXT: this is an allocation-ORDER question, not a placement one. The thing to
 * find is what makes gcc prefer r5 for the first parameter rather than the
 * second -- likely the order in which the two are first REFERENCED after the
 * prologue, which spelling 3 tried to vary at the call and could not, because
 * the call's argument order is itself fixed by the ROM.
 */

extern int iwram_3001ebc;
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_898_2008ef4(int a, int b, int c)
{
    char *p;

    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q2 = 0x80;
        q0 = 0;
        q1 <<= 8;
        q2 <<= 7;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = b;
        q0 = 0;
        q1 = a;
        __Func_809218c(q0, q1, q2);
    }
    p = (char *)iwram_3001ebc;
    p += 0xe4 << 1;
    *(int *)p = 0x10;
    __Func_8091e9c(c);
}
