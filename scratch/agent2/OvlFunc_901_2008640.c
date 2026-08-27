/* OvlFunc_901_2008640  --  NOT MATCHING, 15 differing of 47 (streams same length)
 * ref: asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s
 *
 * IMPROVES the existing park src/non_matching/overlays/2008640.c (17 -> 15)
 * by ONE statement-order change, and everything else in that park's note
 * still stands (the pooled _CONST_2, the r8/r10 assignment, the pool-skip
 * placement being correct here).
 *
 * The park measured six statement orders.  All TWELVE legal orders of
 * (p = &a->f64), (saved = a->f6), (zero = 0), (the OR) were screened here:
 *
 *     pzos 15   zpos 15   pszo 17   spzo 17   szpo 17   posz 18
 *     pozs 18   psoz 20   spoz 20   pzso 21   zpso 21   zspo 21
 *
 * so the best is `p, zero, OR, saved` -- the SAVED READ LAST, which is the
 * one shape the park did not try (it moved saved and zero together).
 *
 * RESIDUE, six instructions in one block, a pure schedule permutation:
 *     rom   mov r2,#6 / ldrsh r1,[r5,r2] / ldr r3,=2 / ldrh / orr / strh
 *           / mov r8,r1 / mov r1,#0 / mov r10,r1
 *     ours  mov r2,#0 / mov r10,r2 / ldr r3,=2 / ldrh / orr / strh
 *           / mov r2,#6 / ldrsh r3,[r5,r2] / mov r8,r3
 * The ROM issues the ldrsh BEFORE the OR and sinks the `mov r8` copy to
 * AFTER it; gcc always emits the copy adjacent to the ldrsh, so no order can
 * have both.  Also measured at 15 or worse: `int saved` instead of `short`,
 * the f6 access as pointer arithmetic instead of a struct member, the +0x64
 * pointer as pointer arithmetic, and -fno-rerun-cse-after-loop /
 * -fno-schedule-insns / -fno-gcse / -fno-strict-aliasing / -fno-caller-saves /
 * -fno-expensive-optimizations (all 15) and -fcall-saved-r4 (47).
 * -fno-schedule-insns2 is 13 -- the best number seen -- but it is not one of
 * this tree's per-file flag groups and it is still not a match.
 *
 * NOTE: the reference keeps its pool inside the function; verify with
 * make compare even on an OK.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_901_2008640(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    unsigned short zero;
    short saved;

    a = __MapActor_GetActor(0xf);
    p = &a->f64;
    zero = 0;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    saved = a->f6;
    __CutsceneStart();
    __MessageID(0x1cb4);
    __MapActor_SetAnim(0xf, 0);
    __Func_8092848(0xf, 0, 2);
    __Func_8093040(0xf, 0, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = zero;
}
