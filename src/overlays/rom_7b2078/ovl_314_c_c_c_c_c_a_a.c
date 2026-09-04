// fakematch
/* OvlFunc_926_200a68c  --  0x0200a68c
 *
 * From goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_a_a.s, which held this function alone.
 *
 * Member of shape group 0 (flat, {bl, bx, lsl, mov, pop, push}), the largest
 * remaining group after arg_interleave_flat was closed in batch 200.
 *
 * TWINS. This function and OvlFunc_922_2009ad0 are INSTRUCTION-FOR-INSTRUCTION IDENTICAL
 * -- same calls, same constants, same argument orders, in two different
 * overlays. One body was written and the other is the same text with the name
 * changed; both matched on the first screen.
 *
 * FOUR CALLS NEED A PIN AND TWO DO NOT, which is the point worth keeping. The
 * ROM's argument orders are not uniform even within one function:
 *
 *     __MapActor_SetSpeed   mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2
 *     __Func_809228c        mov r1 / mov r2 / mov r0
 *     __MapActor_Jump       mov r2 / mov r0 / mov r1
 *     __MapActor_SetAnim    mov r1 / mov r0          <- reversed
 *     __MapActor_SetAnim    mov r0 / mov r1          <- natural, written plainly
 *
 * The two __MapActor_SetAnim calls take their arguments in OPPOSITE orders in
 * the same function. Reading each call off the listing separately is not
 * pedantry here; a pin on the second one would break it, as measured in
 * src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_b.c.
 *
 * The two incoming arguments are held in r5 and r6 across the whole body, which
 * is what the ROM's `push {r5, r6, lr}` pays for, and gcc does that on its own.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_926_200a68c(int a, int b)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xa0;
        q2 = 0xa0;
        q0 = 0;
        q1 <<= 10;
        q2 <<= 9;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = a;
        q2 = b;
        q0 = 0;
        __Func_809228c(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0;
        q0 = 0;
        q1 = 4;
        __MapActor_Jump(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 7;
        q0 = 0;
        __MapActor_SetAnim(q0, q1);
    }
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 6);
    __CutsceneEnd();
}
