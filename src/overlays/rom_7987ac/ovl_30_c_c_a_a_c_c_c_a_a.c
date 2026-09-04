// fakematch
/* OvlFunc_902_200811c  --  0x0200811c
 *
 * From goldensun/asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_c_c_a_a.s, which
 * held this function alone, so no split was needed.
 *
 * A cutscene: emote an actor, wait, bump a counter behind a flag test.
 *
 * PARKED AT 2 OF 62 AS "the documented out-of-reach case". It named itself a
 * test case for the day the interleave class was reached at a STRAIGHT-LINE
 * site, and that is exactly what closed it. The residue was one instruction:
 *
 *     rom   mov r1, #0x81 / mov r0, #0x10 / lsl r1, #0x1 / mov r2, #0x3c
 *     ours  mov r1, #0x81 / lsl r1, #0x1  / mov r0, #0x10 / mov r2, #0x3c
 *
 * The single-instruction argument 0x10 belongs INSIDE the split build of
 * 0x81 << 1. Pinning the three argument registers and assigning them in the
 * ROM order puts it there:
 *
 *     p1 = 0x81;  p0 = 0x10;  p1 <<= 1;  p2 = 0x3c;
 *
 * WHY THE PARK STOPPED, and it reasoned carefully rather than carelessly. It
 * knew the recipe needed a branch dominating the argument setup, checked, and
 * found this function's only conditional sits after the call. Its conclusion
 * followed from the rule as docs/elevation.md then stated it -- that the shape
 * needs a dominating branch -- and that rule was wrong. The branch was only
 * ever a way to make the value dead at the use. Pinning a call-clobbered
 * register arranges the same thing with no branch anywhere. The correction is
 * recorded in the constant-rematerialisation section of docs/elevation.md,
 * measured first on OvlFunc_963_2008334.
 *
 * The park also read its own negative result correctly, and that reading still
 * stands: "name the OTHER arguments" was inert here because the recipe it came
 * from did its naming in a dominating block. The block was the lever and the
 * naming was only how you reached across it. What has changed is that the pin
 * is a second, branchless way to reach the same place.
 *
 * KEPT FROM THE PARK, and load-bearing: __Func_8092c40 has NO declaration.
 * That is what fixed the other half of the original 4-instruction residue --
 * an implicitly-declared callee fills its two arguments in the ROM order
 * where a prototyped one reverses them. It must stay absent; `extern int f();`
 * is not equivalent, it behaves as a full prototype.
 */

extern int iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int n);
extern void __MapActor_DoAnim(int a, int n);
extern void __MapActor_Emote(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_902_200811c(void)
{
    char *p;

    __CutsceneStart();
    __MessageID(0x1cd4);
    __Func_8092848(0x10, 0, 2);
    __MapActor_SetAnim(0x10, 1);
    __Func_8093040(0x10, 0, 0x14);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x10, 0, 0x14);
    {
        register int p0 __asm__("r0");
        register int p1 __asm__("r1");
        register int p2 __asm__("r2");
        p1 = 0x81;
        p0 = 0x10;
        p1 <<= 1;
        p2 = 0x3c;
        __MapActor_Emote(p0, p1, p2);
    }
    __Func_8093040(0x10, 0, 0x1e);
    __Func_8092c40(0x10, 0);
    if (__Func_8091c7c(0, 0) != 0) {
        p = (char *)iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
    }
    __Func_8093040(0x10, 0, 0x14);
    __SetFlag(0xc0 << 2);
    __SetFlag(0x868);
    __CutsceneEnd();
}
