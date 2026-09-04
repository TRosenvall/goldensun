/* OvlFunc_963_2008124  --  0x02008124  [asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c.s]
 *
 * NOT MATCHING. Best screen 104 of 144, LENGTH EXACT at 144 from the first
 * attempt. The candidate kept below is that best form.
 *
 * A gState-driven dispatcher: reads a stage value out of gState and, on two
 * different values, either walks a chain of flag tests or re-animates five
 * actors. Chosen by tools/templated.py at 0.75 over eight shared symbols.
 *
 * The length being exact from the start, and staying exact across three of the
 * four spellings, says the STRUCTURE is right -- the control flow, the call
 * sequence and the stack traffic all agree. What disagrees is register roles
 * and address arithmetic, in at least three independent ways, and they
 * interact: one extra instruction early shifts every later line, which is why
 * the headline count is 104 rather than something small.
 *
 * THREE SEPARATE PROBLEMS, all measured, none closed.
 *
 * 1. THE BASE REGISTER IS CALL-CLOBBERED IN THE ROM AND CALLEE-SAVED IN OURS.
 *    The first block does
 *
 *        rom    ldr r1, =gState ... add r2, r1, r0 / strh r3, [r2]
 *        ours   ldr r5, =gState ... add r3, r5, r1 / strh r2, [r3]
 *
 *    r1 is scratch; r5 is preserved. gcc picks r5 because our gState local is
 *    live across the whole function, so the ROM's first block must have had a
 *    base that DIES at the end of the block. Giving the block its own local
 *    is worth 8 instructions (112 -> 104) and does not finish the job.
 *
 * 2. THE SECOND OFFSET IS DERIVED, NOT REBUILT. Both stores in that block
 *    address gState at 0xe2*2 and 0xe3*2, four bytes apart:
 *
 *        rom    mov r3, #0xe3 / lsl r3, #1 / add r2, r1, r3
 *        ours   add r1, #0x2                                  <- reuses the first
 *
 *    gcc sees the second offset as the first plus two and takes it. The ROM
 *    builds it from scratch. Naming each offset in its own statement does NOT
 *    stop the strength reduction -- it makes things worse (120 of 144, and the
 *    function goes two instructions SHORT at 142).
 *
 * 3. A COMPARISON CONSTANT IS POOLED THAT DOES NOT NEED TO BE.
 *
 *        rom    ldr r3, =0xa9 / cmp r6, r3
 *        ours   cmp r6, #0xa9
 *
 *    0xa9 is 169, well inside the #imm8 range a Thumb `cmp` encodes, so this
 *    is NOT the halfword-store pooling class (blocker 1b) and not a range
 *    problem at all. gcc is right and the ROM went the long way round, which
 *    means the source did not hand it a literal here. Worth its own look:
 *    every other comparison in the function uses the immediate form, including
 *    `cmp r3, #3` and `cmp r3, #5` a few lines later, so whatever produced the
 *    pooled form applies to this one comparison only.
 *
 * FOUR SPELLINGS, MEASURED:
 *
 *     one gState local shared by every block        112 of 144
 *     a block-local base for the first block        104 of 144   <- kept
 *     that, plus each offset named in its own statement
 *                                                   120 of 144, and 142 lines
 *     that, plus the block-local base PINNED to r1  133 of 144, and 145 lines
 *
 * THE PIN MAKES IT WORSE, and that is consistent with where the lever's
 * boundary was measured in batch 194. A pin moves the mov that materialises
 * the pinned value; it does not tell the allocator that the value is DEAD
 * after the block, which is the actual difference here. Pinning a
 * long-lived value into a call-clobbered register asks the allocator for
 * something it then has to undo around every call, and it spends
 * instructions doing so -- the function grows by one.
 *
 * NEXT: problem 1 is the one to solve first, because 2 and 3 may be
 * consequences of the base's lifetime rather than separate choices. The
 * question is how to write a gState base that is dead at the end of a block
 * WITHOUT pinning it. Nothing in docs/elevation.md addresses lifetime
 * directly; the closest entry is the gState-base rule, which says the base
 * must be a named local past offset 255 -- true here and already done, but it
 * says nothing about scope.
 */

extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_963_2008124(void)
{
    unsigned char *g;
    unsigned char *h;
    unsigned char *a;
    int v;
    int t;

    if (__GetFlag(0x89f) != 0) {
        h = gState;
        *(short *)(h + 0xe2 * 2) = 0x69;
        t = 0xa;
        *(short *)(h + 0xe3 * 2) = t;
    }
    g = gState;
    v = *(short *)(g + 0xe0 * 2);
    if (v == 0xa9) {
        if (__GetFlag(0x897) != 0)
            __MapActor_SetPos(0xa, 0, 0);
        if (*(short *)(g + 0xe1 * 2) == 3) {
            if (__GetFlag(0x8fb) != 0) {
                *(short *)(g + 0x90 * 4) = v;
                *(short *)(g + 0x242) = 1;
            }
            if (__GetFlag(0x8fc) != 0) {
                *(short *)(g + 0x90 * 4) = v;
                *(short *)(g + 0x242) = 5;
            }
            __ClearFlag(0x12f);
        }
        g = gState;
        if (*(short *)(g + 0xe1 * 2) == 1) {
            __SetFlag(0x8fb);
            if (__GetFlag(0x96f) == 0)
                __Func_8010704(6, 0, 2, 1, 8, 0x1b);
        }
        if (*(short *)(g + 0xe1 * 2) == 5)
            __SetFlag(0x8fc);
    } else if (v == 0xaa) {
        __MapActor_SetAnim(8, 4);
        __MapActor_SetAnim(9, 4);
        __MapActor_SetAnim(0xa, 3);
        __MapActor_SetAnim(0xb, 4);
        __MapActor_SetAnim(0xc, 3);
        a = __MapActor_GetActor(0xf);
        *(int *)(a + 0x1c) = 0x19999;
        __Func_8010704(0x6c, 0x26, 1, 1, 0x66, 0x38);
    }
}
