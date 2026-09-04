// fakematch
/* OvlFunc_953_200a904  --  0x0200a904
 *
 * From goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_c.s.
 *
 * Member of shape group 0 at the widened 45-instruction cut.
 *
 * THE FIRST CALL IS A PRECOMPUTE BIND WITH THE POOL LOADS SPLIT AROUND `mov r0`:
 *
 *     ldr r2, =0xcccc / mov r0, #0 / ldr r1, =0x19999
 *
 * -- not merely `mov r0` landing last, but the two pool loads themselves
 * emitted r2-then-r1 with the cheap argument between them. Pinning all three
 * and assigning in the ROM's order reaches it; this is the same class as
 * src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c, one step further disordered.
 *
 * The three waypoint calls are the ordinary interleave, identical in shape and
 * differing only in one constant, and the last of them goes to a different
 * helper -- __MapActor_TravelTo rather than __Func_8092158 -- which is worth
 * noticing because it would be easy to write the three from one template and
 * miss the change of callee.
 */

extern void __CutsceneStart(void);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_953_200a904(void)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0xcccc;
        q0 = 0;
        q1 = 0x19999;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapTransitionIn();
    __MapActor_SetAnim(0, 2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc8;
        q2 = 0xd6;
        q0 = 0;
        q1 <<= 2;
        q2 <<= 1;
        __Func_8092158(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xaf;
        q2 = 0xd6;
        q0 = 0;
        q1 <<= 2;
        q2 <<= 1;
        __Func_8092158(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x96;
        q2 = 0xd6;
        q0 = 0;
        q1 <<= 2;
        q2 <<= 1;
        __MapActor_TravelTo(q0, q1, q2);
    }
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x16);
}
