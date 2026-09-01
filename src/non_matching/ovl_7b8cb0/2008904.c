/* OvlFunc_931_2008904 (0x02008904) -- NON-MATCHING.
 * Blocker class: stack-argument materialisation, plus an `orr` destination.
 *
 * 222 lines against 222, SEVEN differing, in two shapes.
 *
 * SHAPE 1, three lines, at the ONE call site of eight where both stack
 * arguments are distinct literals:
 *
 *     rom    mov r3, #0x14 / mov r2, #0x29 / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0x14 / str r3, [sp] / mov r3, #0x29 / str r3, [sp, #4]
 *
 * The other seven sites match, and they match for a reason worth reading: the
 * original code passes a FLAG VARIABLE (which is zero on that path) as both
 * stack arguments rather than a literal zero --
 * `__Func_8010704(0x40, 0, 0x20, 0x20, f1, f1)` inside the `else` of
 * `if (f1 != 0)`. A value already live in a register needs no move, so gcc's
 * one-register habit never shows.
 *
 * SHAPE 2, four lines, at the LAST use of each of two shared mask constants:
 *
 *     rom    orr r5, r3 / strb r5, [r0]      (third use, mask register dies)
 *     ours   orr r3, r5 / strb r3, [r0]
 *
 * The first two uses of each mask match. On the third the ROM makes the MASK's
 * register the destination, because it is dead afterwards; gcc keeps using the
 * loaded value's register. `*p = 8 | *p;` is canonicalised to `*p |= 8;` and is
 * byte-identical.
 *
 * MEASURED (rom 222 lines, all at exact length):
 *   baseline                                              222, 10
 *   `px`/`py` for the two SetPos constants assigned in
 *     the entry block (the batch-176 basic-block lever)   222, 7   <- best
 *   `*p = 8 | *p;` at the third use only                  222, 7   (inert)
 *   the two masks ALSO named in the entry block           236, 232 (four locals
 *                          is too many -- gcc spills, exactly as
 *                          ovl_7b9cb4/200a6c0.c records for eight)
 *   -fno-schedule-insns / -fno-gcse / -fno-strength-reduce
 *     / -fno-defer-pop                                    222, 7   (inert)
 *   -fno-schedule-insns2                                  222, 52  (worse)
 *
 * THE LEVER WORKED AND THEN RAN OUT, in one function. Two locals in the
 * dominating block fixed the `__MapActor_SetPos` interleave (10 -> 7); adding
 * two more to fix the masks cost fourteen lines and 232 differing. That is the
 * bound recorded in ovl_7b9cb4/200a6c0.c, now measured at its edge: **two
 * dominating-block locals fit in this function and four do not.**
 *
 * WHAT IS RIGHT: the four-way flag cascade with its cross-jumped
 * `__DeleteFieldActor` tails; the named `gState` base for the halfword at
 * `0xe1 << 1`; `0xb4 << 17` and `0xa8 << 16`; the actor pointers ADVANCED IN
 * PLACE (`p = __MapActor_GetActor(0x16) + 0x59;`); and passing a flag variable
 * as a stack argument where the ROM does, which is what makes seven of the
 * eight six-argument calls come out exactly.
 *
 * NEXT: nothing source-level in eight probes.
 */
extern unsigned char gState[];
extern char *__MapActor_GetActor(int slot);
extern int __GetFlag(int);
extern void __ClearFlag(int);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __DeleteFieldActor(int slot);
extern void __Func_8091ff0(int a);
extern void __Func_8092950(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern int __StartTask(void *fn, int pri);
extern void OvlFunc_931_2008d08(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(char *a, int f);
extern void __Func_8092b08(int a, int b);
extern void __WaitFrames(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);

void OvlFunc_931_2008904(void)
{
    char *a;
    char *p;
    unsigned char *g;
    int f1;
    int f2;
    int f3;
    int px;
    int py;

    px = 0xb4 << 17;
    py = 0xa8 << 16;
    a = __MapActor_GetActor(0);
    f1 = __GetFlag(0x242);
    if (f1 != 0) {
        __CopyMapTiles(0x40, 0x20, 0, 0x20, 0x20, 0x20);
        __Func_8010704(0x40, 0x20, 0x20, 0x20, 0, 0);
        __DeleteFieldActor(0x14);
        __DeleteFieldActor(0x15);
    } else {
        f2 = __GetFlag(0x241);
        if (f2 != 0) {
            __CopyMapTiles(0x40, 0, 0, 0x20, 0x20, 0x20);
            __Func_8010704(0x40, 0, 0x20, 0x20, f1, f1);
            __DeleteFieldActor(0x11);
            __DeleteFieldActor(0x14);
            __DeleteFieldActor(0x15);
        } else {
            f3 = __GetFlag(0x90 << 2);
            if (f3 != 0) {
                __CopyMapTiles(0, 0x40, 0, 0x20, 0x20, 0x20);
                __Func_8010704(0, 0x40, 0x20, 0x20, f2, f2);
                __DeleteFieldActor(0x10);
                __DeleteFieldActor(0x11);
                __DeleteFieldActor(0x15);
            } else {
                __Func_8010704(0, 0x20, 0x20, 0x20, f3, f3);
                __DeleteFieldActor(0xf);
                __DeleteFieldActor(0x10);
                __DeleteFieldActor(0x11);
            }
        }
    }
    if (__GetFlag(0x8ff) != 0) {
        __DeleteFieldActor(0x12);
    } else {
        __Func_8091ff0(0xaa);
        __Func_8092950(0x12, 2);
        __MapActor_SetAnim(0x12, 3);
        __StartTask(OvlFunc_931_2008d08, 0xc8 << 4);
    }
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        __ClearFlag(0x12f);
    __Func_8010704(0, 0x21, 4, 3, 0x14, 0x29);
    if (__GetFlag(0x906) != 0)
        __MapActor_SetPos(0x13, px, py);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
    __Func_8092950(0x16, 0xf);
    __Func_8092950(0x17, 0xf);
    __Func_8092950(0x18, 0xf);
    p = __MapActor_GetActor(0x16) + 0x59;
    *p |= 8;
    p = __MapActor_GetActor(0x17) + 0x59;
    *p |= 8;
    p = __MapActor_GetActor(0x18) + 0x59;
    *p |= 8;
    p = __MapActor_GetActor(0x16) + 0x23;
    *p |= 2;
    p = __MapActor_GetActor(0x17) + 0x23;
    *p |= 2;
    p = __MapActor_GetActor(0x18) + 0x23;
    *p |= 2;
    __Func_8092b08(0x16, 1);
    __Func_8092b08(0x17, 1);
    __Func_8092b08(0x18, 1);
    __WaitFrames(1);
    __CutsceneStart();
    __Func_80933f8(*(int *)(a + 8), *(int *)(a + 0xc), *(int *)(a + 0x10), 0);
    __Func_800fe9c();
    __CutsceneEnd();
    __WaitFrames(1);
}
