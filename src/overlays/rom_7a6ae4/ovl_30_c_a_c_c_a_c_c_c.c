/* OvlFunc_920_2008304  --  0x02008304
 *
 * The whole of goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_c_c_c.s: one
 * function, no data.
 *
 * The block puzzle's check, re-run whenever a block moves. Slots 0x0b and 0x0c
 * are the blocks; each is tested against the target tile (0x23, 0x17) at
 * whole-tile resolution and its own save bit is set or CLEARED to match, which
 * is what lets a wrong move be undone. The gate then follows the OR of the two,
 * with 0x302 tracking which state it is currently in so the transition fires
 * once per change rather than every frame.
 *
 * BUILT WITH CSE_CFLAGS, and this is the OTHER member of the pair the [join]
 * marker cannot tell apart. The marker fired on 0x302, materialised four times
 * with labels between the repeats -- the same picture as OvlFunc_941_2008210.
 * It is not the same thing, and the distinguishing test is WHAT IS REPEATED:
 *
 *   a repeated `mov rN, #imm8` feeding a STACK-ARGUMENT slot
 *       -> a second source VARIABLE; split the local (the 941 lever)
 *   a repeated POOLED FLAG ID consumed as r0 by a `bl`
 *       -> one source literal, commoned by gcc's rerun-CSE pass; the source
 *          cannot reach it, and CSE_CFLAGS is the answer
 *
 * Four constant-facing spellings confirm the second half here -- separate named
 * locals, the equal spelling `0x181 << 1`, explicit `== 0`, and goto-raised
 * label use counts all leave the SAME 6 instructions in 4 regions. Constant
 * propagation folds any name back to the same const_int, so FOR A POOLED
 * CONSTANT THERE IS NO SOURCE-LEVEL SPLIT: the count-of-materialisations rule
 * is about variables, not about literals.
 *
 * A refinement of the recorded guard/set note: the pair does NOT have to be in
 * the same block or even the same arm. Here the GetFlag guards a block and the
 * SetFlag sits after the join, and the pass still commons them -- twice, once
 * per arm of the outer if/else. Flag-bisecting is the fast diagnosis, and
 * -fno-gcse, -fno-cse-follow-jumps, -fno-thread-jumps and
 * -fno-expensive-optimizations all leave the four `mov r0, r6` in place. SO
 * -fno-gcse NOT HELPING IS POSITIVE EVIDENCE FOR CSE_CFLAGS, not evidence that
 * the shape is unreachable.
 *
 * 0x304 is the pooled-small-constant tell and the ROM builds it, so it is
 * spelled `0xc1 << 2`; 0x302 and 0x303 cannot be built and are plain literals.
 * The 0x24 stack argument is free -- inline, one shared local, or two per-arm
 * locals all match once the flag is right, because gcc CSEs the literal into a
 * register within each arm on its own.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_DoAnim(int slot, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_920_2008304(void)
{
    unsigned char *a;
    unsigned char *b;

    a = __MapActor_GetActor(0xb);
    b = __MapActor_GetActor(0xc);
    if ((*(int *)(a + 8) >> 20) == 0x23 && (*(int *)(a + 0x10) >> 20) == 0x17)
        __SetFlag(0x303);
    else
        __ClearFlag(0x303);
    if ((*(int *)(b + 8) >> 20) == 0x23 && (*(int *)(b + 0x10) >> 20) == 0x17)
        __SetFlag(0xc1 << 2);
    else
        __ClearFlag(0xc1 << 2);
    if (__GetFlag(0x303) || __GetFlag(0xc1 << 2)) {
        if (!__GetFlag(0x302)) {
            __CutsceneStart();
            __CutsceneWait(0x28);
            __PlaySound(0xd2);
            __MapActor_DoAnim(0x11, 6);
            __Func_8010704(0, 1, 1, 1, 0x24, 0x16);
            __Func_8010704(0, 2, 1, 1, 0x24, 0x18);
            __CutsceneEnd();
        }
        __SetFlag(0x302);
    } else {
        if (__GetFlag(0x302)) {
            __CutsceneStart();
            __CutsceneWait(0x28);
            __PlaySound(0xdc);
            __MapActor_DoAnim(0x11, 2);
            __Func_8010704(1, 1, 1, 1, 0x24, 0x16);
            __Func_8010704(1, 2, 1, 1, 0x24, 0x18);
            __CutsceneEnd();
        }
        __ClearFlag(0x302);
    }
}
