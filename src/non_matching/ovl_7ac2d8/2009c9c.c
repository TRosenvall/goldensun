/*
 * OvlFunc_924_2009c9c -- asm/overlays/rom_7ac2d8/ovl_f84_c_c_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: gcc CSEs the flag id 0x256 into a callee-saved register across the
 * guard chain. 63 lines against 62 -- ONE OVER, and the one is the extra
 * `push {r6}` with its pop. The ROM emits `ldr r0, =0x256` fresh at the
 * __GetFlag and again at the __SetFlag.
 *
 * REACHABLE -- do not record this as a wall. 47 matching functions load the
 * same pool slot into r0 for both __GetFlag and __Set/__ClearFlag, and 10 of
 * those are in functions that DO push callee-saved registers, including
 * OvlFunc_882_200ad28 which pushes exactly {r5, lr} like this one. So neither
 * plain literals nor a callee-saved frame is what stops it.
 *
 * WHAT THE MATCHING EXAMPLES HAVE THAT THIS DOES NOT: a control-flow JOIN
 * between the two uses. OvlFunc_882_200ad28's __GetFlag sits in an if/else
 * condition and its __SetFlag comes after the arms merge; the one that pushes
 * only lr has two GetFlags and a long straight run to the SetFlag. This
 * function's guards all branch to the EPILOGUE, so the __SetFlag is plainly
 * dominated by the __GetFlag with no merge in between, and gcc's cse reaches
 * straight through.
 *
 * TRIED AND REJECTED, all byte-identical at 63 lines:
 *   * separate named locals per use site -- the lever that closed
 *     OvlFunc_948_20095f0
 *   * the second use written as `0x100 + 0x156` (gcc folds it)
 *   * the guards rewritten as early returns instead of nested ifs, which also
 *     matches the ROM's branch layout better
 *
 * The next thing to try is a spelling that puts a real join between the two
 * uses without changing the branch targets -- but note the ROM's own control
 * flow has no such join, so that may not be the answer either.
 */
extern unsigned char L6064[] __asm__(".L6064");
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_924_2009c9c(void)
{
    unsigned char *e;
    unsigned char *p;
    unsigned int x;
    int y;
    int lo;
    int one;

    if (__GetFlag(0x256) != 0)
        return;
    x = *(short *)(__MapActor_GetActor(0) + 0xa);
    x -= 0xa4;
    y = *(short *)(__MapActor_GetActor(0) + 0x12);
    if (x > 7)
        return;
    lo = 0xba << 1;
    if (y < lo)
        return;
    if (y >= lo + 8)
        return;
    __CutsceneStart();
    __SetFlag(0x256);
    __CutsceneWait(5);
    e = __MapActor_GetActor(0);
    *(int *)(e + 0xc) += 0xfffe0000;
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x3c) = *(int *)(__MapActor_GetActor(0) + 0xc);
    one = 1;
    __CopyMapTiles(6, 0x1d, 0xa, 0x17, one, one);
    __PlaySound(0xd9);
    __Func_8010560(L6064, 0xa, 0x12);
    __CutsceneEnd();
}
