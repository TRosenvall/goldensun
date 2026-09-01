/* OvlFunc_927_200a1b0 (0x0200a1b0) -- NON-MATCHING.
 * Blocker class: ARGUMENT INTERLEAVE, STRAIGHT-LINE variant (not reachable).
 *
 * 108 lines against the ROM's 108, SIX differing, and all six are the same
 * shape at three call sites -- `mov r0, #0x12` has to land BEFORE the `lsl`
 * that completes r1's split build, and ours lands after:
 *
 *     rom   mov r1,#0x88 / mov r2,#0xb4 / lsl r2,#17 / mov r0,#0x12 / lsl r1,#16
 *     ours  mov r1,#0x88 / mov r2,#0xb4 / lsl r2,#17 / lsl r1,#16 / mov r0,#0x12
 *
 * The three sites are __MapActor_SetPos(0x12, 0x88<<16, 0xb4<<17),
 * __Func_8092adc(0x12, 0xc0<<8, 0x28) and __MapActor_Surprise(0x12, 0x81<<1).
 * Everything else -- 102 of 108 instructions, including seven other calls with
 * split-constant arguments and the eight-argument OvlFunc_927_2008ae8 with its
 * four stack slots -- is exact on the FIRST screen.
 *
 * WHY IT IS NOT REACHABLE.  This function has no conditional branch at all.
 * docs/elevation.md's argument-interleave note is explicit that the guard is
 * load-bearing: naming the split builds works because gcc will not hold the
 * constants across a branch and so rematerialises them at the call, and doing
 * the same in a straight-line function makes gcc HOLD them instead. That is
 * what tools/guarded_interleave.py was written to separate, and this function
 * is on the wrong side of it. It should not have passed the filter -- the
 * filter counts calls and instructions, not guards.
 *
 * MEASURED, all at exactly 108 lines and 6 differing unless noted:
 *   - naming both split builds in locals immediately before the call, the
 *     spelling that works on guarded sites                            6
 *   - naming the PRE-SHIFT base (`b = 0x88; f(0x12, b << 16, ...)`)   6
 *   - the same with `volatile int b` to block the fold        112 lines, 105
 *   - declaring all three callees `int` instead of `void` (the return-type
 *     lever, which is the documented control over r0's position)      6
 *   - withholding the prototypes, all three and Surprise alone        6
 *   - -fno-schedule-insns, -fno-rerun-cse-after-loop, -fno-gcse,
 *     -fno-strict-aliasing, -fno-defer-pop, -fno-expensive-optimizations
 *                                                                     6
 *   - -fno-schedule-insns2                              first diff at 1, 52
 *
 * A READING WORTH KEEPING, because it narrows the shape. The ROM does NOT put
 * r0 early everywhere: the four FOUR-argument OvlFunc_927_2008d90 calls emit
 * `mov r0, #0x12` last, after every shift, and those all match. Only the two-
 * and three-argument calls interleave. So the shape is not "this ROM likes r0
 * early" -- it is specific to calls that leave r3 free, which is consistent
 * with the interleave being an argument-loading order and not a scheduling
 * artifact. That also explains why no scheduler flag touches it.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT.  The constant 0xc0 << 10 is built across
 * __CutsceneWait -- `mov r5, #0xc0` before the call, `lsl r5, #10` after -- and
 * a plain named `int k` assigned before the call reproduces that exactly,
 * including r5 being the register. The four stack arguments of
 * OvlFunc_927_2008ae8 come out right from bare literals; the ROM's separate
 * `mov r4, #0` for the three stack zeros and `mov r3, #0` for the register zero
 * needs no naming at all.
 *
 * NEXT: nothing source-level. This is a specimen for the straight-line
 * interleave, not a candidate.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int a, int b, int c);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_927_200a1b0(void)
{
    unsigned char *e;
    int k;

    e = __MapActor_GetActor(0x12);
    __CutsceneStart();
    __MapActor_SetPos(0x12, 0x88 << 16, 0xb4 << 17);
    OvlFunc_927_2008ea8(0x12, 1);
    OvlFunc_927_2008d90(0x12, 0x88, 0xcc << 1, 0x80 << 12);
    __CutsceneWait(0xa);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 11), 0,
                        0, 0, 1, 0);
    __Func_8092adc(0x12, 0xc0 << 8, 0x28);
    __MapActor_Surprise(0x12, 0x81 << 1);
    __Func_80925cc(0x12, 2);
    __SetCameraTarget(0x12, 1);
    OvlFunc_927_2008d90(0x12, 0x88, 0xdc << 1, 0xc0 << 11);
    __Func_809280c(0, 0x12, 0);
    k = 0xc0 << 10;
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0x12, 0x88, 0xec << 1, k);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0x12, 0x88, 0xfc << 1, k);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    __SetCameraTarget(0, 1);
    __MapActor_SetPos(0x12, 0, 0);
    __CutsceneWait(0x3c);
    __SetFlag(0x89d);
    __CutsceneEnd();
}
