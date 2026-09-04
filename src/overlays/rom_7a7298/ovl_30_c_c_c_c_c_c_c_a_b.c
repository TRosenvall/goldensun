// fakematch
/* OvlFunc_921_20099bc  --  0x020099bc
 *
 * From goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_a.s.
 *
 * Member of the "arg_interleave_flat" class, marked resolved in
 * src/non_matching/overlays/arg_interleave_flat.c. The ROM writes `mov r0`
 * INSIDE r1's two-instruction build:
 *
 *     mov r1, #0x80 / mov r0, #0 / lsl r1, #10 / ldr r2, =0x1999
 *
 * Pinning the argument registers and assigning them in the ROM's order places
 * it. Matched on the first screen, as every member of this class has.
 *
 * This member has a different BODY from the rest of the class -- a cutscene
 * open and a transition rather than a sound and a table blit -- which is a
 * reminder that the class was defined by its OPCODE SET, not by what the
 * functions do. The blocker is identical all the same.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapTransitionIn(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092158(int a, int b, int c);

void OvlFunc_921_20099bc(void)
{
    __CutsceneStart();
    __MapTransitionIn();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 0;
        q1 <<= 10;
        q2 = 0x1999;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __Func_8092158(0, 0xe8, 0xcc);
    __CutsceneEnd();
}
