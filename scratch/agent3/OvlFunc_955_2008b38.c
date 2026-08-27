/* OvlFunc_955_2008b38 -- PARKED.  ref: asm/overlays/rom_7ddb88/ovl_30_c_c_c_c.s
 *
 * As written: 29 differing of 63, first diff at position 0.  Two constants are
 * each used twice inside the single label-free region before the `if`:
 * 0x80<<17 (both __MapActor_SetPos calls) and 0x80<<7 (both __Func_809280c
 * calls).  gcc hoists both into r5/r6, which adds a callee-saved register to
 * the prologue.  This is the pool/computed-constant CSE class and the function
 * has no boundary BEFORE the uses, so the basic-block lever cannot reach it.
 * Measured identical at 29 of 63: -fno-rerun-cse-after-loop, -fno-gcse,
 * -fno-cse-follow-jumps, -fno-expensive-optimizations.
 *
 * WHAT IS LEFT UNDERNEATH: with the second use of each constant changed to
 * 0x81<<17 / 0x81<<7 so they are no longer CSE-able, the same source is
 * 12 differing of 63 (first diff at 11), and every one of them is the
 * arg-interleave shape -- the ROM splits `mov rN,#K / lsl rN,#S` around the
 * OTHER argument movs at three call sites (__MapActor_SetPos x2,
 * __Func_809280c, and the `lsl r2,#0x10` in __Func_80933f8) and gcc emits each
 * pair contiguously.  That is the basic-block lever's class too, and it also
 * has no dominating boundary here.  So this function needs TWO independent
 * things the tree cannot currently produce for a straight-line prologue.
 */
extern void __DeleteFieldActor(int id);
extern void __Func_807808c(int a);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int n);
extern void __WaitFrames(int n);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_common1_fac(int a);

void OvlFunc_955_2008b38(int a)
{
    __DeleteFieldActor(0x28);
    __DeleteFieldActor(0x29);
    __Func_807808c(1);
    __CutsceneStart();
    __MapActor_SetPos(8, 0xb0 << 15, 0x80 << 17);
    __MapActor_SetPos(0, 0xf0 << 15, 0x80 << 17);
    __Func_809280c(8, 0x80 << 7, 0);
    __Func_809280c(0, 0x80 << 7, 0);
    if (a < 0) {
        __MapActor_SetAnim(8, 0xa);
        __MapActor_SetAnim(0, 0x23);
    } else {
        __MapActor_SetAnim(8, 8);
        __MapActor_SetAnim(0, 0x1c);
    }
    __WaitFrames(1);
    __Func_80933f8(0xd0 << 15, 0, 0xc0 << 16, 0);
    OvlFunc_common1_fac(a);
    __CutsceneEnd();
}
