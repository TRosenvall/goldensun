/* OvlFunc_971_2008b94 -- 0x02008b94, last of the four functions that were in
 * asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a.s.  The split cuts the tail;
 * OvlFunc_971_2008580 stays unattempted and OvlFunc_971_20087b0 and
 * OvlFunc_971_2008860 stay parked, all three in the `_a` part.  No data section.
 *
 * 468 bytes, 175 encodings.  objcmp reports one differing encoding and one extra
 * relocation, and they are the same fact: the pool word at 0x1b4 carries
 * R_ARM_ABS32 _MSG_2930 here and the resolved 0x2930 in the reference.  The other 51
 * relocations are identical, in the reference's order.  The compare against baserom
 * is the proof.
 *
 * _MSG_2930's CONTROL IS AN ENCODING LIMIT, AND IT IS EXACT.  The ROM holds the base
 * in r6 for the whole function and derives EIGHT ids as `add r0, r6, #K` for K in
 * 1..7, then reaches the NINTH -- 0xd past the base -- as its own `ldr r0, =0x293d`,
 * in this function, feeding the same __MessageID.  Thumb's `add rd, rn, #imm3`
 * encodes only 0..7: every id the short form CAN reach is derived, and the one id it
 * CANNOT reach is pooled.  That is what a symbol base predicts and what an integer
 * cannot produce, since use_related_value (cse.c:1637) fires for a CONST and never
 * for a bare CONST_INT.  As a plain `0x2930` the function is 177 of 175 differing at
 * 180 instructions and 492 bytes -- five instructions and 24 bytes long, each id
 * becoming its own constant.
 *
 * A `void` CALLEE DECLARED `int` FIXED THREE ARGUMENT-ORDER SITES AT ONCE, where
 * precompute was inert.  Three `__Func_8092c40(8, 0)` sites came out `mov r0,#8 /
 * mov r1,#0` against the ROM's `mov r1,#0 / mov r0,#8` -- and the ROM uses the OTHER
 * order at its three remaining sites.  Block-scoped precompute locals in both orders
 * were fully inert; declaring __Func_8092c40 `int` went 9 -> 3.  At a
 * two-register-argument call, reach for the callee's return type BEFORE the
 * precompute.
 *
 * A `goto` OUT OF AN IF-ARM ONLY PLACES THE BLOCK LATE IF THE ARM DOES NOT FALL
 * THROUGH.  `if (OvlFunc_971_200808c(0) == 0) goto Ld26;` was inverted by gcc, which
 * pulled the two-instruction block inline; writing the OTHER arm as the `then` and
 * letting the short block fall out after it gives the ROM's `beq .Ld26`, worth 20
 * encodings.  The bare `goto` is right for a LONG arm -- `goto Lc74` was worth
 * 154 -> 9 here -- and wrong for a short one.
 *
 * Also load-bearing: a named `g = gState` assigned AFTER __CutsceneStart() (151 of
 * 175, 170 instructions, otherwise); `int` return on this function itself (3); and
 * `msg = m + 7;` before the `strh` (3).
 *
 * SHIMS, BOOKED: three `__asm__ ("" : "+r" (fN));` barriers, worth 162, 103 and 13
 * respectively.  Inert and therefore not written: `int` on __MessageID, __SetFlag,
 * __ClearFlag, OvlFunc_971_2008128, __Func_809280c, __SetFlagByte and __WaitFrames.
 */
extern int _MSG_2930;
extern unsigned char gState[];

extern void __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern void __Func_809280c(int a, int b, int c);
extern int OvlFunc_971_200808c(int i);
extern void OvlFunc_971_2008128(int n);
extern void OvlFunc_971_200803c(void);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __SetFlagByte(int id, int v);
extern void __MessageID(int id);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

int OvlFunc_971_2008b94(void)
{
    int m;
    int r;
    int msg;
    int f;
    int f2;
    int f3;
    unsigned char *g;

    m = (int)&_MSG_2930;
    __CutsceneStart();
    g = gState;
    __Func_809280c(8, *(int *)(g + (0xfa << 1)), 0);
    if (OvlFunc_971_200808c(0) == 0)
        __WaitFrames(1);
    if (OvlFunc_971_200808c(0) == 0) {
        OvlFunc_971_2008128(5);
        OvlFunc_971_200803c();
        f = 0x173;
        __asm__ ("" : "+r" (f));
        if (__GetFlag(f) != 0)
            goto Lc74;
        __MessageID(m + 5);
        __Func_8092c40(8, 0);
        r = __Func_8091c7c(0, 0);
        if (r == 0) {
            __SetFlagByte(0xfa << 2, 0);
            __SetFlag(0x173);
            __ClearFlag(0xb9 << 1);
            __ClearFlag(0xb6 << 1);
            __SetFlag(0x202);
            msg = m + 7;
            *(short *)(g + 0x2aa) = r;
            goto Lc76;
        }
        __ClearFlag(0x173);
        __SetFlag(0xb6 << 1);
        OvlFunc_971_2008128(0);
        msg = m + 6;
        goto Lc76;
    }
    f2 = 0x173;
    __asm__ ("" : "+r" (f2));
    if (__GetFlag(f2) != 0) {
        OvlFunc_971_2008128(0);
        __MessageID(0x293d);
        __Func_8092c40(8, 0);
        __ClearFlag(0x202);
        __ClearFlag(0x173);
    }
    if (__GetFlag(0x202) == 0)
        goto Lc84;
Lc74:
    msg = m + 3;
Lc76:
    __MessageID(msg);
Lc7a:
    __Func_8092c40(8, 0);
    goto Ld3c;
Lc84:
    if (__GetFlag(0x201) != 0)
        goto Lcb2;
    f3 = 0xc0 << 2;
    __asm__ ("" : "+r" (f3));
    if (__GetFlag(f3) != 0)
        goto Lcb2;
    __MessageID(m);
    __Func_8092c40(8, 0);
    __SetFlag(0xc0 << 2);
    goto Ld3c;
Lcb2:
    __SetFlag(0xc0 << 2);
    if (__GetFlag(0x201) != 0)
        __MessageID(m + 2);
    else
        __MessageID(m + 1);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) != 0)
        goto Ld2e;
    if (OvlFunc_971_200808c(0) != 0) {
        __SetFlag(0xb6 << 1);
        __SetFlag(0xb9 << 1);
        if (__GetFlag(0x201) != 0)
            __MessageID(m + 3);
        else
            __MessageID(m + 4);
        OvlFunc_971_2008128(1);
        __SetFlag(0x202);
        goto Lc7a;
    }
    __SetFlag(0x205);
    goto Ld3c;
Ld2e:
    __MessageID(m);
    __Func_8092c40(8, 0);
Ld3c:
    __CutsceneEnd();
}
