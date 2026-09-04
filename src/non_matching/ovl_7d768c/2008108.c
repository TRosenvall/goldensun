/* OvlFunc_952_2008108  --  0x02008108  [asm/overlays/rom_7d768c/ovl_30_c_a_a_a_c.s]
 *
 * NOT MATCHING. Best 36 of 136, LENGTH EXACT. The candidate kept below is that
 * form. Two of the three levers this function needs are found; the third is not.
 *
 * A two-armed cutscene selected by a save flag, with a position test inside the
 * first arm. The .s holds a second function, so a split is needed when this is
 * finished.
 *
 * LEVER 1, LANDED, AND IT IS THE FINDING WORTH KEEPING: GCC NARROWS AN AND MASK
 * WHEN THE RESULT IS TRUNCATED, AND `~0x3fff` BLOCKS IT WHERE `0xffffc000` DOES
 * NOT. The ROM computes
 *
 *     ldrh r3, [r0, #6] / add r3, #0x2000 / ldr r2, =0xffffc000 / and r3, r2
 *     lsl r3, #16 / asr r6, r3, #16
 *
 * -- mask with the full 32-bit constant, THEN sign-extend from 16 bits. Written
 * as `(short)((*(unsigned short *)(p + 6) + (0x80 << 6)) & 0xffffc000)` gcc
 * emits `ldr r2, =0xc000`. It is not wrong: the value is at most 17 bits and
 * the `(short)` throws bits 16 and up away, so combine propagates the
 * truncation BACKWARDS through the AND and drops the high half of the mask.
 * Spelling the same constant as `~0x3fff` produces the ROM's word. The NOT is
 * opaque to that narrowing where the literal is not.
 *
 * This is a new member of a family already in the tree -- the `short` versus
 * `unsigned short` store that decides whether 0x8000 reaches the pool
 * sign-extended. Same lesson from the other end: WHAT LANDS IN THE POOL IS
 * DECIDED BY THE TYPES AROUND THE CONSTANT, NOT BY THE CONSTANT.
 *
 * LEVER 2, LANDED. `0x80 << 2` is a flag id used THREE times -- GetFlag, then
 * SetFlag in one arm and ClearFlag in the other -- and gcc caches it in r5,
 * widening the prologue. An r0 pin before each of the three calls forces the
 * rebuild the ROM has. This is the ordinary rematerialisation lever and it
 * behaves exactly as documented.
 *
 * WHAT REMAINS -- WHERE `e = 0x80 << 7` IS COMPUTED. The ROM builds it BETWEEN
 * the two operands of the following compare and the compare itself:
 *
 *     mov r2, #0x80 / lsl r3, r6, #16 / lsl r2, #23 / mov r6, #0x80 / lsl r6, #7
 *     cmp r3, r2 / bne .L188
 *
 * The value is not used until after the branch, so gcc sinks it past the `bne`
 * and the function comes out 134 lines with ~100 differing. MEASURED:
 *
 *     plain `e = 0x80 << 7;` before the if          100 differing, 134 lines
 *     a volatile asm on e after the assignment       36 differing, 136 lines
 *     `register int e __asm__("r6")`                 50 differing, 135 lines,
 *                                                    and the prologue widens
 *
 * The barrier is the best of the three and fixes the length, but it over-shoots:
 * it forces the build to the TOP of the block, ahead of `lsl r3, r6, #16`, where
 * the ROM has it after. It also takes r3 rather than the ROM's r6. So the
 * requirement is a build placed at a specific point INSIDE another expression's
 * evaluation, which a statement-level barrier cannot express -- it has two
 * positions available, before or after, and the ROM wants neither.
 *
 * The r6 pin is the obvious answer and it fails for a specific reason worth
 * recording: r6 ALREADY HOLDS `d` at that point, and the ROM's trick is that d's
 * last use is the `lsl r3, r6, #16` immediately before, so the register is free
 * for reuse one instruction later. Pinning `e` to r6 makes the two live ranges
 * overlap in the source where the ROM has them adjacent, so `d` is pushed to r7
 * and the prologue widens.
 *
 * NEXT: the question is how to spell "this value's live range begins exactly
 * where that one's ends" when the two are separate source variables. A single
 * variable reused for both is the obvious try and was not made.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_952_2008108(int slot)
{
    int d;
    int e;
    int m;
    register int p0 __asm__("r0");

    d = (short)((*(unsigned short *)(__MapActor_GetActor(0) + 6) + (0x80 << 6))
                & ~0x3fff);
    __CutsceneStart();
    __Func_808e118();
    p0 = 0x80; p0 <<= 2;
    if (__GetFlag(p0) == 0) {
        p0 = 0x80; p0 <<= 2;
        __SetFlag(p0);
        __ClearFlag(0x969);
        __MessageID(0x1ff7);
        __ActorMessage(slot, 0);
        __CutsceneWait(0xa);
        e = 0x80 << 7;
        __asm__ volatile ("" : : "r" (e));
        if ((d << 16) == (0x80 << 23)) {
            __Func_80921c4(0, 0x28, 0x68);
            __Func_8092adc(0, 0, 0);
        }
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = slot; q1 <<= 9; q2 <<= 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0x30; q0 = slot; q1 = 0; q2 = -q2; __Func_8092304(q0, q1, q2); }
        __Func_8092304(slot, 0x40, 0);
        __Func_8092adc(slot, e, 0);
    } else {
        p0 = 0x80; p0 <<= 2;
        __ClearFlag(p0);
        __SetFlag(0x969);
        { PIN3; q1 = 0x80; q0 = slot; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __Func_80921c4(0, 0x78, 0x60);
        { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0; q0 = 0; __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        m = 0x1ff8;
        __MessageID(m);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0; q0 = slot;
            __Func_8092c40(q0, q1);
        }
        if (__Func_8091c7c(0, 0) == 0) {
            __MessageID(m + 1);
            __ActorMessage(slot, 0);
        } else {
            __MessageID(m + 2);
            __ActorMessage(slot, 0);
        }
        __CutsceneWait(0xa);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 3; q0 = slot;
            __MapActor_DoAnim(q0, q1);
        }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0x40; q0 = slot; q1 = -q1; q2 = 0; __Func_8092304(q0, q1, q2); }
        __Func_8092304(slot, 0, 0x30);
    }
    __CutsceneEnd();
}
