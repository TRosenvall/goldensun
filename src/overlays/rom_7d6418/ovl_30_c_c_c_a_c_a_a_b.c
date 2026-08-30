/* Cluster OvlFunc_951_20088f8..OvlFunc_951_20088f8 extracted from
 * goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a.s.
 *
 * Total .text for this TU = 256 bytes.
 *
 * REQUIRES `_MSG_e23 = 0xe23;` in message.sym. Four levers, 89 of 98 down to
 * exact, and three of them were lifted off the sibling
 * src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_b.c in this same directory:
 *
 *   - THE MESSAGE BASE IS A SYMBOL, NOT A LITERAL. The ROM does
 *     `ldr r7, =0xe23` then `add r0, r7, #2/#4/#5`. A plain `int base = 0xe23`
 *     is constant-propagated wherever it is assigned; `(int)(&_MSG_e23)` is
 *     what makes gcc spend the callee-saved register. That is the
 *     `push {r5, r6, r7}` versus `{r5, r6}` difference, worth 3 instructions.
 *   - THE FOUR __Func_8092c40 SITES DISAGREE about argument order, so two of
 *     them go through an __asm__ alias with its own declaration.
 *   - `ldr r0, =0x89` IS A POOLED-CONSTANT TELL. gcc builds 137 with `mov`,
 *     so a pooled 0x89 means a symbol -- _AREA_89. Verified against the
 *     assembler that Thumb-1 gas does NOT fold `ldr rX, =imm8` into a `mov`,
 *     so this is a real difference and never a disassembly artifact.
 *   - EVERY MESSAGE ARM ENDS WITH ITS OWN CALLS. Routing the three terminal
 *     ids through one variable and a single join lets gcc hoist one of the
 *     pool loads above its compare, giving a function two instructions SHORTER
 *     than the ROM. Writing the tail out in all three arms and letting
 *     cross-jumping share it gives the ROM's layout.
 */
extern int _MSG_e23;
extern int _AREA_89;

extern int __Func_8078b60(int a);
extern void __Func_808ba38(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8019908(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int C40I(int a, int b) __asm__("__Func_8092c40");
extern int __Func_8091c7c(int a, int b);
extern int __Func_8078550(void);
extern void __SetDestMap2(int a, int b);
extern void __Func_8091f90(int a, int b);

void OvlFunc_951_20088f8(int a)
{
    int h;
    int v;
    int m;

    h = __Func_8078b60(0xe4);
    __Func_808ba38();
    if (a == 0) {
        m = (int)(&_MSG_e23);
        __MessageID(m);
        __ActorMessage(8, 0);
        if (h == 0)
            goto done;
        __MessageID(m + 2);
        __Func_8019908(h, 5);
        C40I(8, 0);
        if (__Func_8091c7c(0, 0) != 0)
            goto done;
        v = __Func_8078550();
        if (v == 0) {
            __MessageID(m + 4);
            __Func_8092c40(8, 0);
        } else {
            if (v > 6)
                goto bad;
            __MessageID(m + 5);
            __Func_8092c40(8, 0);
        }
        if (v > 6)
            goto bad;
        if (__Func_8091c7c(0, 0) == 0)
            goto bad;
        __MessageID(0xe29);
        __ActorMessage(8, 0);
        return;
    } else if (h == 0) {
        __MessageID(0xe32);
        __ActorMessage(8, 0);
        return;
    } else {
        __MessageID(0xe33);
        C40I(8, 0);
        if (__Func_8091c7c(0, 0) == 0)
            goto bad;
        __MessageID(0xe31);
        __ActorMessage(8, 0);
        return;
    }
done:
    __ActorMessage(8, 0);
    return;
bad:
    __MessageID(0xe2a);
    __ActorMessage(8, 0);
    __SetDestMap2(0xfe << 1, 0);
    __Func_8091f90((int)(&_AREA_89), 0xc);
}
