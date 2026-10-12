/* OvlFunc_881_2009a98 -- NOT MATCHING, a LENGTH gap: ref 48 instructions against ours 46 (-2).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/N9a98_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 2 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_77a7c8/2009a98.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_c.s --func OvlFunc_881_2009a98
 *
 * The original prose park is preserved below.
 */
extern unsigned char gScript_881__0200d1b8[];
extern unsigned char gScript_881__0200d158[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetCameraTarget(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern int OvlFunc_881_200b41c(void);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int n);

void OvlFunc_881_2009a98(void)
{
    unsigned char *e;

    e = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    *(int *)(e + 0x18) = 0xa0 << 9;
    *(int *)(e + 0x1c) = 0xa0 << 9;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    __MapActor_SetSpeed(8, 0x9999, 0x4ccc);
    e += 0x64;
    *(short *)e = 0;
    if (OvlFunc_881_200b41c() == 0xb)
        __MapActor_SetBehavior(8, gScript_881__0200d1b8);
    else
        __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*(short *)e == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x6a);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_881_2009a98 -- 0x02009a98,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a.s
 *
 * 67 lines against the ROM's 69, and the whole gap is the FIRST CALL.
 * Candidate at scratch/N9a98_best.c.
 *
 * BLOCKER: the `-1` TRIPLE, `__Func_80933f8(-1, -1, -1, 0)`.
 *      rom   mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0 / neg r1 / neg r2 / neg r0
 *      ours  mov r2,#1 / neg r2,r2 / mov r3,#0 / mov r0,r2 / mov r1,r2
 * gcc builds one -1 and copies it; the ROM materialises three independently.
 * Two instructions shorter, and every line after it is shifted, which is where
 * the 63 differing comes from -- the body itself is right.
 *
 * This is the class in src/non_matching/overlays/constant_reuse.c, and this
 * function is a CLEAN instance of it: unlike OvlFunc_955_2009424, which also
 * calls __Func_80933f8 elsewhere and so has the constant CSE'd across two call
 * sites and into a callee-saved register, here there is exactly ONE call and
 * nothing else in the function touches -1.  So the reuse is not cross-site CSE;
 * gcc simply will not materialise the same constant three times for three
 * argument registers.  That narrows the class usefully: whatever the original
 * build did differently, it is inside the expansion of a single call.
 *
 * SCREENED, all identical at 63 differing: three separate `int n1, n2, n3 = -1`
 * locals assigned immediately before the call; deleting the callee's prototype
 * so the arguments go through default promotion; and declaring the parameters
 * with three different types (`int, long, short`) so the constants would have
 * different modes at expansion -- they are folded to the same CONST_INT before
 * that and CSE sees one value either way.
 *
 * The eleven-flag sweep in constant_reuse.c was not repeated here.
 *
 * Everything else in the function is confirmed by the shifted-but-identical
 * tail: the mutated `e += 0x64` pointer that is both stored through and then
 * polled with `ldrsh`, the do-while around __WaitFrames, and the two-script
 * branch on OvlFunc_881_200b41c() == 0xb.
 *)

*/
#endif
