// fakematch
/* OvlFunc_953_200a4d8  --  0x0200a4d8
 *
 * Cut out of goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c.s.
 *
 * Thirteen __Func_8092adc calls in two runs, an actor fetched once and written
 * twice, and a message id. Most of it is the ordinary pinned-argument idiom.
 * The two field stores are what needed work.
 *
 * BOTH OPERANDS OF THE STORE HAVE TO BE PINNED. The ROM is
 *
 *     mov r2, r5 / mov r3, #0xb4 / add r2, #0x64 / lsl r3, #2 / strh r3, [r2]
 *
 * -- two interleaved chains, the ADDRESS in r2 and the VALUE in r3. Every
 * ordinary spelling produces that interleave with the two registers SWAPPED,
 * and the swap does not respond to source order: naming the address before the
 * value, after the value, and moving the declaration between the two all give
 * exactly 7 differing. That tie is over one skeleton and is therefore weak
 * evidence by the standard this tree already applies to ties -- but it is a
 * true tie, and the pin is what settles it. `register short *p __asm__("r2")`
 * with `register int w __asm__("r3")` is exact. Pinning is cheap here because
 * the ROM uses no high register; in a function that spills, it would not be
 * (see the boundary recorded in docs/elevation.md this same round).
 *
 * This is the same shape as the accumulate in
 * src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c -- naming one operand leaves the
 * allocator to choose the other and it chooses wrong -- extended from a
 * read-modify-write to a plain store.
 *
 * TWO SMALLER ONES, both costing an instruction each until they were written as
 * statements. `0xb4 << 2` inline goes to the pool; `w = 0xb4; w <<= 2;` gives
 * the ROM's mov+lsl. And `*(short *)a = 0x70` pools 0x70 EVEN THOUGH IT FITS AN
 * 8-BIT IMMEDIATE -- `v = 0x70;` as its own statement gives `mov r3, #0x70`.
 * The second is worth noting because the usual explanation for a pooled
 * constant is that it does not fit, and here it does; the trigger is the store
 * width, not the magnitude. That spelling also removed a stray `b` to the next
 * label, which was two of the extra instructions.
 *
 * `a += 0x66` is an in-place advance, not an offset: the ROM issues
 * `add r5, #0x66` and stores through r5 afterwards.
 */
extern void OvlFunc_953_2009c48(int a);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int n);
extern void __MessageID(int id);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_953_200a4d8(void)
{
    unsigned char *a;
    int v;

    a = __MapActor_GetActor(0xd);
    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 2; q0 = 8;
        __Func_80925cc(q0, q1);
    }
    __MapActor_SetIdle(0xd);
    __WaitFrames(1);
    { PIN3; q1 = 0xe0; q2 = 0; q0 = 0; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xd, 1);
    { PIN3; q1 = 0xd0; q0 = 0xc; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xd, 0, 0);
    { PIN3; q1 = 0x80; q0 = 0xe; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0xf; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 0x11; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q2 = 0; q1 <<= 8; q0 = 0x12; __Func_8092adc(q0, q1, q2); }
    __MessageID(0x2112);
    OvlFunc_953_2009c48(8);
    __MapActor_DoAnim(0, 3);
    {
        register short *p __asm__("r2");
        register int w __asm__("r3");
        p = (short *)(a + 0x64);
        w = 0xb4;
        w <<= 2;
        *p = w;
    }
    a += 0x66;
    v = 0x70;
    *(short *)a = v;
    __MapActor_SetBehavior(0xd, 2);
    { PIN3; q1 = 0xc0; q0 = 0xc; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 0xe; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0xf; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x10, 0, 0);
    { PIN3; q1 = 0xa0; q0 = 0x11; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0x12; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneEnd();
}
