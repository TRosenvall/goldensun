/* OvlFunc_945_200b364 (0x0200b364) -- 177 encodings, 440 bytes, exact.
 *
 * FAKEMATCH: `__asm__ ("" : "+r" (g))` emits zero bytes but carries the match.
 *
 * It must go on the FIRST, DOMINATING `__GetFlag(0x928)` call.  On the second
 * site it is worth nothing (3 of 177, unchanged).  cse records the equivalence
 * when it processes the DEFINING site, so a barrier there means there is never
 * an equivalence to propagate.  This corrects the standing note that a "+r"
 * barrier is inert on a repeated constant -- it is POSITIONAL, not inert.
 *
 * What makes the shared constant cost an instruction is local-alloc.c:886's
 * `REG_N_REFS (regno) == 2` rule: two pseudos with two references each both
 * rematerialise into a bare `ldr r0, =0x928`, while one pseudo with three
 * references misses that test and survives in a callee-saved register.
 *
 * Two further constructs, both deliberate:
 *   - `{ int k = 0x80; p[0x59] |= k; }` -- the CARRIER is the handle, not the
 *     spelling.  `0x80 | p[0x59]`, `p[0x59] | 0x80` and `p[0x59] |= 0x80` are
 *     byte-identical, because commutative_operand_precedence canonicalises the
 *     CONST_INT into operand 1 regardless.  What flips the two-address `orr`'s
 *     register roles is the operand being a PSEUDO rather than a CONST_INT.
 *     Worth 2 of 177.
 *   - the expensive arguments to __MapActor_SetSpeed are named at the TOP of the
 *     function (3 of 177), because commoning needs room.  Contrast the sibling
 *     park, where lowering one cheap argument's LUID needs an ADJACENT
 *     block-scope local instead.
 *   - `*(short *)(r+6) = 0xc0 << 6` as a bare HImode literal both pools AND
 *     sign-extends (`ldr r3, =0xffffb000`); an int carrier gives the ROM's
 *     `mov`/`lsl`.  The literal form is 109 of 177 and 16 bytes long.
 *
 * Measured and inert: three flag pairs (-fno-gcse with -fno-cse-follow-jumps and
 * with -fno-expensive-optimizations, -fno-cse-follow-jumps with
 * -fno-thread-jumps), so this figure is NOT flag-conditional.  An int carrier
 * for the pooled zero is also inert.
 */
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void OvlFunc_945_200c5d0(void);
extern void OvlFunc_945_200c890(int slot, int a, int b, int c);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200812c(void);
extern void OvlFunc_945_2008284(void);

void OvlFunc_945_200b364(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    unsigned char *t;
    int f;
    int v;
    int sx;
    int sy;

    sx = 0xcccc;
    sy = 0x6666;
    if (__GetFlag(0x911) == 0) {
        OvlFunc_945_200c5d0();
        return;
    }
    {
        int g = 0x928;
        __asm__ ("" : "+r" (g));
        if (__GetFlag(g) != 0)
            OvlFunc_945_200c5d0();
    }
    f = __GetFlag(0x93e);
    if (f != 0)
        return;
    if (__GetFlag(0x8a << 4) != 0) {
        p = __MapActor_GetActor(9);
        OvlFunc_945_200c8e8(0xd, 0, 0);
        OvlFunc_945_200c890(8, 0xe4 << 1, 0xa3 << 2, 0);
        OvlFunc_945_200c890(9, 0xf0 << 1, 0x96 << 2, 0xb0 << 8);
        __MapActor_SetSpeed(9, sx, sy);
        *(short *)(p + 0x66) = f;
        p[0x63] = 0;
        { int k = 0x80; p[0x59] |= k; }
        *(void **)(p + 0x6c) = (void *)OvlFunc_945_200812c;
        q = __MapActor_GetActor(8);
        q[0x62] = 0;
        *(void **)(q + 0x6c) = (void *)OvlFunc_945_2008284;
        if (__GetFlag(0x109) != 0)
            OvlFunc_945_200c890(0, 0xf0 << 1, 0x29a, 0xa0 << 8);
    } else if (__GetFlag(0x928) != 0) {
        OvlFunc_945_200c890(8, 0xde << 1, 0x266, 0xd0 << 8);
        OvlFunc_945_200c8e8(0xd, 0, 0);
    } else if (__GetFlag(0x925) != 0) {
        OvlFunc_945_200c890(8, 0xe4 << 1, 0xa2 << 2, 0);
        OvlFunc_945_200c8e8(0xd, 0, 0);
    } else if (__GetFlag(0x921) != 0) {
        OvlFunc_945_200c890(8, 0x1db, 0x256, 0x80 << 8);
        OvlFunc_945_200c890(9, 0xe7 << 1, 0x26a, 0xb0 << 8);
        r = __MapActor_GetActor(0xc);
        v = 0xc0 << 6;
        *(short *)(r + 6) = v;
        t = __MapActor_GetActor(0xb);
        v = 0xb0 << 8;
        *(short *)(t + 6) = v;
        OvlFunc_945_200c890(0xd, 0xdb << 1, 0x293, 0xd0 << 8);
        OvlFunc_945_200c890(0xa, 0xf4 << 1, 0xac << 2, 0xb0 << 8);
    }
}
