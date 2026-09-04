// fakematch
/* OvlFunc_966_200810c  --  0x0200810c
 *
 * From goldensun/asm/overlays/rom_7f148c/ovl_30_c_c_a_c_c_a.s, which held this
 * function alone, so no split was needed.
 *
 * PARKED AT 5 OF 27, in two places and both the same shape -- the ROM puts the
 * FIRST argument in the middle of another argument's construction:
 *
 *     rom   mov r1,#0x80 / mov r2,#0x80 / mov r0,#0x12 / lsl r1,#9 / lsl r2,#8
 *     rom   mov r1,#0x10 / mov r0,#0x12 / neg r1,r1 / mov r2,#0
 *
 * The second site is the same shape with a `neg` in place of the shift, which
 * is the sub-case the discriminator in src/non_matching/ovl_7ebdfc/2008120.c
 * predicts will yield: the interleaved argument sits against a two-instruction
 * build on a DISTINCT value, so there is an operation whose order the source
 * can set. Pinning at both sites matches.
 */

extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_966_200810c(void)
{
    __SetFlag(0x9bb);
    __MessageID(0x28b8);
    __ActorMessage(0x12, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q2 = 0x80;
        q0 = 0x12;
        q1 <<= 9;
        q2 <<= 8;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x10;
        q0 = 0x12;
        q1 = -q1;
        q2 = 0;
        __Func_8092304(q0, q1, q2);
    }
    __Func_8092adc(0x12, 0, 0);
    __CutsceneWait(0xa);
}
