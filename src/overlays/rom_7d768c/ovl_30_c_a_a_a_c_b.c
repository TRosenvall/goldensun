// fakematch
/* OvlFunc_952_2008108  --  0x02008108
 *
 * Cut out of goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_a_a_c.s.
 *
 * WAS PARKED at 36 of 136 one batch ago, on a requirement stated there as
 * unspellable: a constant whose live range must begin exactly where another's
 * ends. That park closed by naming the one thing not tried -- "a single
 * variable reused for both is the obvious try and was not made" -- and that is
 * the whole fix.
 *
 * ONE VARIABLE, TWO RANGES. The ROM computes the compare's left operand out of
 * r6 and then immediately refills r6 with the next constant:
 *
 *     mov r2, #0x80 / lsl r3, r6, #16 / lsl r2, #23 / mov r6, #0x80 / lsl r6, #7
 *     cmp r3, r2 / bne .L188
 *
 * As two source variables this cannot be reached. Written with the second value
 * assigned back into the FIRST variable, after its last read has been hoisted
 * into a local of its own, it falls out with no lever at all:
 *
 *     t = d << 16;
 *     d = 0x80 << 7;
 *     if (t == (0x80 << 23)) { ... }
 *
 * 36 of 136 to 45 of 135 -- which looks worse and is not: the first arm went
 * exact and the residue moved to the second arm, ninety instructions later.
 * THE LESSON IS THE ONE FROM LAST BATCH ARRIVING FROM THE OTHER SIDE: a
 * differing count is only meaningful for the first divergence, and a lever that
 * moves the first divergence forward has worked even when the total rises.
 *
 * The rest is ordinary and already documented. The message base in the second
 * arm is `register int m __asm__("r6")` because the ROM feeds three __MessageID
 * calls from one register with `add r0, r6, #1` and `#2`; without the pin
 * constant propagation folds all three to their own pool entries. Its pool load
 * then hoisted two statements early and took one `do { } while (0)` behind it.
 * The flag id `0x80 << 2` is used three times across both arms and is rebuilt
 * at each with an r0 pin.
 *
 * `~0x3fff` RATHER THAN `0xffffc000`, and this one is worth carrying. gcc
 * NARROWS AN AND MASK WHEN THE RESULT IS TRUNCATED: the value is at most 17
 * bits and the `(short)` throws bits 16 and up away, so combine propagates the
 * truncation backwards through the AND and emits `ldr r2, =0xc000`. The NOT
 * form is opaque to that and produces the ROM's full word. Same family as the
 * `short` versus `unsigned short` store lever, from the other end.
 *
 * TRYC REJECTED THIS FUNCTION AND WAS WRONG, which is the other thing to
 * record. It reported "OUR POOL HAS 5 entries and the reference needs 4". Its
 * reference count was `set()` of the `=value` operands across the WHOLE file,
 * but this ROM has TWO pools -- an explicit `.pool_aligned` after the
 * `b .L160`, and the implicit one inside `.func_end`. Thumb PC-relative loads
 * are forward-only, so `0x969`, used once on each side of that boundary, needs
 * a slot in BOTH pools; the reference needs five as well. tools/tryc.py has
 * been fixed to count per pool. `make compare` was the authority here, exactly
 * as it was the last time this tree trusted a screen over the build.
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
    int t;
    register int m __asm__("r6");
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
        t = d << 16;
        d = 0x80 << 7;
        if (t == (0x80 << 23)) {
            __Func_80921c4(0, 0x28, 0x68);
            __Func_8092adc(0, 0, 0);
        }
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = slot; q1 <<= 9; q2 <<= 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0x30; q0 = slot; q1 = 0; q2 = -q2; __Func_8092304(q0, q1, q2); }
        __Func_8092304(slot, 0x40, 0);
        __Func_8092adc(slot, d, 0);
    } else {
        p0 = 0x80; p0 <<= 2;
        __ClearFlag(p0);
        __SetFlag(0x969);
        { PIN3; q1 = 0x80; q0 = slot; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __Func_80921c4(0, 0x78, 0x60);
        { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0; q0 = 0; __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        do { } while (0);
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
