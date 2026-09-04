// fakematch
/* OvlFunc_898_20092c0  --  0x020092c0
 *
 * Cut out of goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_c_c.s.
 *
 * The same cutscene beat as OvlFunc_901_2008d84 in rom_797990, driving actor
 * slot 0x13 rather than 0x12. Set a walk speed, send the actor to one mark,
 * face it, jump three times with a wait between each, send it to a second mark,
 * face it again, and record the scene with flag 0x858.
 *
 * THE TWO ARE THE SAME ROUTINE. Normalising the slot constant and the function
 * name, the two disassemblies diff to nothing -- every other constant, every
 * instruction, identical. So this file is the earlier one with 0x12 changed to
 * 0x13, and it matched on the first screen with no probing at all.
 *
 * That was worth checking rather than assuming: two cutscenes that look alike
 * usually differ somewhere in their marks or waits, and a normalised diff of
 * the two bodies costs one command and either saves the whole round or catches
 * the difference before it becomes a wrong candidate.
 *
 * templated.py scored this 1.00 against ovl_314_c_c_a_c_a_a_a.c -- the file
 * elevated in the previous round. The tool now feeds on its own output: every
 * function landed becomes a template for its kin, which is why this one cost
 * one screen where the original cost eight.
 *
 * The scaffolding is inherited and its justification is recorded there in full:
 * 0x80 << 7 feeds two __Func_8092adc calls in a branchless function so gcc
 * commons it into a callee-saved register; the two __Func_80921c4 shifts land a
 * slot early written inline; and the last pin must be declared UNINITIALISED so
 * its assignment can be placed after the shift rather than at the declaration.
 * Each piece was measured by removal on the original at 3, 2, 2 and 1
 * instructions respectively.
 */

extern void __Func_8078a08(int a);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int slot, int a);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __SetFlag(int id);

void OvlFunc_898_20092c0(void)
{
    __Func_8078a08(0xe7);
    __CutsceneStart();
    __CutsceneWait(0xa);
    __Func_80925cc(0x13, 2);
    {
        register int s0 __asm__("r0") = 0x13;
        register int s1 __asm__("r1") = 0xcccc;
        register int s2 __asm__("r2") = 0x6666;
        __MapActor_SetSpeed(s0, s1, s2);
    }
    {
        register int a2 __asm__("r2") = 0xcc;
        register int a1 __asm__("r1") = 0xd8;
        register int a0 __asm__("r0") = 0x13;
        a2 <<= 1;
        __Func_80921c4(a0, a1, a2);
    }
    __CutsceneWait(0xa);
    {
        register int p0 __asm__("r1") = 0x80;
        register int p1 __asm__("r0") = 0x13;
        register int p2 __asm__("r2");
        p0 <<= 7;
        p2 = 0x14;
        __Func_8092adc(p1, p0, p2);
    }
    __MapActor_Jump(0x13, 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x13, 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x13, 6, 0);
    __CutsceneWait(0x1e);
    {
        register int b2 __asm__("r2") = 0xc4;
        register int b1 __asm__("r1") = 0xd8;
        register int b0 __asm__("r0") = 0x13;
        b2 <<= 1;
        __Func_80921c4(b0, b1, b2);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(0x13, 0x80 << 7, 0x14);
    __SetFlag(0x858);
    __CutsceneEnd();
}
