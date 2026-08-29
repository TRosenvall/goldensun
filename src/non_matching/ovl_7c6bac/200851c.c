/* OvlFunc_942_200851c  [ovl_7c6bac]  --  0x0200851c
 *
 * Source asm: goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_a.s
 *
 * A 147-instruction cutscene with real control flow: 30 calls, 8 branches, a
 * five-way nest, a price compared against the party's coins, and two separate
 * join points. Attempted deliberately as the control-flow half of the
 * large-function experiment -- see reports/large-functions.md.
 *
 * RESULT: 35 instructions in disagreeing regions, of 147. Seventy-six percent
 * right. As with the straight-line half, the length is not what stops it.
 *
 * TWO THINGS MOVED IT AND BOTH ARE ABOUT SHAPE, NOT ABOUT LENGTH:
 *
 *   first transcription, natural `if/else` and early `return`s   78 of 147
 *   + `goto` to the ROM's two join points                        42 of 147
 *   + -fno-rerun-cse-after-loop                                  35 of 147
 *
 * THE JOIN POINTS ARE THE BIG ONE, and it is a general lesson for functions
 * this size. The ROM has TWO exits -- one that runs __CutsceneEnd and one that
 * does not -- and five paths that reach them. Written with early `return`s,
 * gcc emitted __CutsceneEnd three times and laid the blocks out differently,
 * for 43 instructions of disagreement. Written with `goto` to two labels
 * mirroring the ROM's `.L660` and `.L664`, that is halved.
 *
 * A short function has one exit and the question never arises. This is the
 * first place where the SHAPE OF THE CONTROL-FLOW GRAPH, rather than any
 * individual instruction, was the dominant error -- and it is a lever, not a
 * blocker: it cost one screen to find and it is mechanical to apply.
 *
 * THE FLAG IS READ AND WRITTEN, so this needs CSE_CFLAGS. 0x8a5 is passed to
 * __GetFlag at the top and __SetFlag near the bottom; at -O2 gcc hoists it into
 * a callee-saved register, which here costs an extra high register (r10) and a
 * wider push/pop. This would be the EIGHTH TU on that flag.
 *
 * WHAT REMAINS, all four of them known classes or scheduling:
 *
 * 1. THE THREE POINTER WALKS, ~15 instructions. The ROM advances the base
 *    destructively -- `mov r3,#0xec / ldr r2,[r7] / lsl r3,#1 / add r2,r3` --
 *    and gcc emits the three-operand `add r2, r3, r1`. TRIED: the offset as a
 *    named local shared by all three sites (WORSE, 44 -- it keeps 0x1d8 live in
 *    r8 for the whole function and pushes the price to r10), and `base +=
 *    0xec << 1` in place (35, no change). The named-intermediate lever that
 *    works on a two-instruction address does not reach a three-instruction one.
 *
 * 2. BLOCK PLACEMENT. The ROM puts the shared `__ActorMessage(8, 0)` tail
 *    between two arms; gcc puts it after both. Same graph, different layout.
 *
 * 3. `price` REMATERIALISED. The ROM keeps 0x258 in r8 and passes `mov r0, r8`;
 *    gcc rebuilds it with `mov r0,#0x96 / lsl r0,#2` at one of the two uses.
 *    The mirror image of the `-1` problem in the other half of this
 *    experiment -- gcc holds what the ROM rebuilds there, and rebuilds what the
 *    ROM holds here. Nothing in the tree moves either.
 *
 * 4. Two single-instruction displacements of a `mov r0, #imm` inside argument
 *    blocks.
 *
 * NOT PRESENT: any failure mode that does not also occur in short functions.
 * That is the result worth carrying.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8019908(unsigned int a, int b);
extern void *__CreateUIBox(int a, int b, int c, int d, int e);
extern void __Func_801e7c0(int a, void *box, int c, int d);
extern void __Func_801ea08(unsigned int a, int b, void *box, int d, int e);
extern void __CloseUIBox(void *box, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_8091a58(int a, int b);
extern void __AddCoins(int n);

void OvlFunc_942_200851c(void)
{
    unsigned int price;
    unsigned char *g;
    unsigned short *p;
    void *box;

    price = 0x96 << 2;
    __CutsceneStart();
    if (__GetFlag(0x8a5)) {
        __MessageID(0x1d0b);
        __ActorMessage(8, 0);
        goto done;
    }
    __MessageID(0x1d04);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) == 1) {
        __Func_8093040(8, 0, 0xa);
        goto end;
    }
    p = (unsigned short *)((unsigned char *)iwram_3001ebc + (0xec << 1));
    *p = *p + 1;
    __Func_8019908(price, 5);
    __Func_8092c40(8, 0);
    box = __CreateUIBox(0x13, 8, 0xb, 4, 2);
    __Func_801e7c0(0xc8a, box, 0, 0);
    g = (unsigned char *)&gState;
    __Func_801ea08(*(unsigned int *)(g + 0x10), 6, box, 0x18, 8);
    if (__Func_8091c7c(-1, 0) == 1) {
        __CloseUIBox(box, 2);
        __MapActor_DoAnim(0, 4);
        __CutsceneWait(0xa);
        goto after;
    }
    if (price > *(unsigned int *)(g + 0x10)) {
        __CloseUIBox(box, 2);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0xa);
        p = (unsigned short *)((unsigned char *)iwram_3001ebc + (0xec << 1));
        *p = *p + 1;
        __PlaySound(0x71);
        goto after;
    }
    __CloseUIBox(box, 2);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0xa);
    p = (unsigned short *)((unsigned char *)iwram_3001ebc + (0xec << 1));
    *p = *p + 3;
    __ActorMessage(8, 0);
    __Func_8091a58(0xeb, 0);
    __SetFlag(0x8a5);
    __AddCoins(-price);
    goto end;
after:
    __ActorMessage(8, 0);
end:
    __CutsceneEnd();
done:
    ;
}
