/* WHOLE-FILE CONVERSION of asm/overlays/rom_7b4558/ovl_30_c_c_c_a_c.s --
 * OvlFunc_927_200a500 is its only function, no data section, so no split and no
 * linker change.  617 instructions, 1700 bytes, 661 encodings and 163 relocations
 * identical.
 *
 * `_AREA_44`, `_AREA_45` and `_AREA_46` are ALREADY in area.sym (lines 76-78) and
 * nothing needed adding.  The ROM does `ldr r3, =0x44 / cmp r2, r3` three times
 * where `cmp r2, #0x44` would do, which is area.sym's own criterion; spelling them
 * `area == (int)&_AREA_44` is what closes the function.  Against the plain reference
 * this object reports 3 differing encodings plus three extra R_ARM_ABS32 -- those
 * three pool words and nothing else, the object-level AREA shape already documented
 * in this overlay's own park src/non_matching/ovl_7b4558/200a2c0.c.
 *
 * ================================================================
 * A `|=` ON A STRUCT BYTE PICKS ITS DESTINATION REGISTER BY THE MODE OF THE
 * CONSTANT CARRIER, NOT BY SOURCE OPERAND ORDER
 * ================================================================
 *
 * New, and worth 8 encodings across four sites.  The ROM emits
 * `ldrb r2,[r0] / mov r3,#8 / orr r3,r2` -- THE CONSTANT IN THE DESTINATION.
 * Measured on `p[0x59] |= 8`:
 *
 *   p[0x59] |= 8                        22 real / 21 of 661
 *   p[0x59] = 8 | p[0x59]               identical
 *   int b = 8; p[0x59] |= b             identical
 *   unsigned int b = 8; ...             identical
 *   unsigned char b = 8; r[0x59] |= b    6 real /  9 of 661
 *
 * SO SOURCE OPERAND ORDER IS INERT -- `iorsi3`'s `%` constraint lets reload swap
 * either way and RTL canonicalisation erases the distinction.  THE DISCRIMINATOR IS
 * QImode AGAINST SImode ON THE CARRIER, exactly parallel to the recorded HImode
 * constant-store rule.  "Write the constant first" is measurably NOT the cure, which
 * is the trap this finding replaces.
 *
 * A STACK ARGUMENT THAT SURVIVES THE r0-r3 FILLS NEEDS A CALLEE-SAVED PIN.
 * OvlFunc_927_2008244(0, 0x23, 0x1d, 1, 4, 0): the ROM puts the 0 in R5 and does
 * `str r5,[sp,#4]` AFTER filling r0-r3.  Every `int`-local spelling put it in r2 and
 * stored it first -- block-scoped, function-scoped, either declaration order, all
 * 17-18 of 661.  `{ register int q5 __asm__("r5"); q5 = 0; f(..., 4, q5); }` gives
 * 3 of 661, all four permutations of the pin score the same, and dropping it costs
 * 10.  THE FIRST CALLEE-SAVED-REGISTER PIN MEASURED TO PAY IN THIS CLASS.
 *
 * Two `extern int` return types on void callees -- the "r0 last" lever documented in
 * the file-neighbour src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_a.c -- are
 * load-bearing here twice: 24 -> 22 and 21 -> 17, and reverting either costs 7.
 *
 * `x >>= 20;` as its own statement is load-bearing: folding it back gives `ours
 * 1696` against 1700.
 *
 * Ships 12 of 14 pin blocks after a greedy ladder.  All five non-pin constructs cost
 * 7-11 encodings or a size delta when reverted.
 *
 * No per-file Makefile rule matches this stem: rom_7b4558's rules are all
 * ovl_30_c_c_a_c_a%, which this stem does not match.
 */
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern int _AREA_44;
extern int _AREA_45;
extern int _AREA_46;

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned char gScript_927__0200b084[];

extern int OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_927_20088c0(int a);
extern int OvlFunc_927_2008a4c(int a, int b, int c, int d);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);
extern void OvlFunc_927_2009c34(void);
extern void OvlFunc_927_200ac0c(int a);

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_80925cc(int slot, int a);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int slot, int a, int b);

int OvlFunc_927_200a500(void)
{
    unsigned char *p;
    unsigned char *g;
    int area;
    int x;
    int y;
    int f;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;
    g = gState;
    area = *(short *)(g + (0xe0 << 1));
    if (area == (int)&_AREA_44) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 1:
        case 2:
        case 3:
        case 4:
            { PIN1; q0 = 0x89c; f = __GetFlag(q0); }
            if (!f) {
                __CutsceneStart();
                __WaitFrames(1);
                __Func_8092950(0xa, 1);
                { PIN3; q1 = 0xb8; q2 = 0xf0; q0 = 0xa; q1 <<= 15; q2 <<= 15; __MapActor_SetPos(q0, q1, q2); }
                { PIN3; q1 = 0xd0; q0 = 0xa; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
                { PIN3; q0 = 0; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
                __Func_809218c(0, 0x88, 0x40);
                __MapTransitionIn();
                __WaitMapTransition();
                __MapActor_WaitMovement(0);
                __CutsceneWait(0x1e);
                { PIN3; q1 = 0x80; q0 = 0xa; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
                __Func_80925cc(0xa, 2);
                __CutsceneWait(0x1e);
                OvlFunc_927_2008d90(0xa, 0x88, 0x74, 0xe0 << 11);
                OvlFunc_927_2008e18(0xa);
                __Func_8092950(0xa, 0xf);
                p = __MapActor_GetActor(0xa);
                __Actor_SetSpriteFlags(p, 0);
                __SetFlag(0x89c);
                __CutsceneWait(0x3c);
                __CutsceneEnd();
            }
            if (__GetFlag(0x109)) {
                if (!__GetFlag(0xc0 << 2)) {
                    __Func_8092950(0xa, 0xf);
                    { PIN3; q1 = 0x88; q2 = 0xe8; q0 = 0xa; q1 <<= 16; q2 <<= 15; __MapActor_SetPos(q0, q1, q2); }
                }
            }
            break;
        case 7:
        case 8:
        case 9:
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(0x10, *(int *)(p + 8), *(int *)(p + 0x10));
            p = __MapActor_GetActor(0x10);
            *(int *)(p + 0x6c) = 0;
            if (__GetFlag(0x109)) {
                p = __MapActor_GetActor(0x10);
                *(int *)(p + 0xc) = 0x80 << 14;
            }
            __WaitFrames(1);
            __MapActor_SetPos(0x10, 0x9e << 18, 0xdc << 17);
            if (!__GetFlag(0xfd4))
                OvlFunc_927_200ac0c(0x10);
            __Func_8092950(0xb, 0xf);
            __Func_8092950(0xc, 0xf);
            p = __MapActor_GetActor(0xb);
            __Actor_SetSpriteFlags(p, 0);
            p = __MapActor_GetActor(0xc);
            __Actor_SetSpriteFlags(p, 0);
            OvlFunc_927_20088c0(8);
            if (!__GetFlag(0xc4 << 2)) {
                OvlFunc_927_20088c0(9);
            } else {
                __WaitFrames(1);
                { PIN3; q1 = 0x84; q2 = 0xcc; q0 = 9; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                __MapActor_SetAnim(9, 4);
                { int s1a = 0x1f; int s1b = 0x19;
                  __Func_8010704(0x26, 0x1b, 4, 2, s1a, s1b); }
                __MapActor_GetActor(9)[0x23] = 2;
            }
            break;
        }
    } else if (area == (int)&_AREA_45) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 3:
        case 4:
        case 5:
        case 6:
            if (!__GetFlag(0x303)) {
                __Func_8092950(0xc, 0xf);
                p = __MapActor_GetActor(0xc);
                __Actor_SetSpriteFlags(p, 0);
            }
            if (!__GetFlag(0xc1 << 2)) {
                __Func_8092950(0xd, 0xf);
                p = __MapActor_GetActor(0xd);
                __Actor_SetSpriteFlags(p, 0);
            }
            break;
        case 0xa:
        case 0xb:
        case 0xc:
            if (!__GetFlag(0x311)) {
                OvlFunc_927_20088c0(0xa);
            } else {
                __WaitFrames(1);
                { PIN3; q1 = 0x8a; q2 = 0xff; q0 = 0xa; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                __MapActor_SetAnim(0xa, 4);
                __MapActor_GetActor(0xa)[0x23] = 2;
                { int s2a = 0x22; int s2b = 0x1e;
                  __Func_8010704(0x2c, 0x1e, 2, 4, s2a, s2b); }
                { register int q5 __asm__("r5"); q5 = 0;
                  OvlFunc_927_2008244(0, 0x23, 0x1d, 1, 4, q5); }
            }
            OvlFunc_927_20088c0(8);
            OvlFunc_927_20088c0(9);
            p = __MapActor_GetActor(0xb);
            x = *(int *)(p + 8);
            p = __MapActor_GetActor(0xb);
            x >>= 20;
            OvlFunc_927_2008244(2, x, *(int *)(p + 0x10) >> 20, 1, 1, 0xff);
            __WaitFrames(1);
            __Func_8092950(0xb, 6);
            { unsigned char *r = __MapActor_GetActor(8); unsigned char b = 8; r[0x59] |= b; }
            if (!__GetFlag(0x306)) {
                __Func_8092950(0xe, 0xf);
                p = __MapActor_GetActor(0xe);
                __Actor_SetSpriteFlags(p, 0);
                if (__GetFlag(0x305)) {
                    { PIN3; q1 = 0xd4; q2 = 0xf0; q0 = 0xe; q1 <<= 17; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                    { PIN3; q1 = 0xd4; q2 = 0xf0; q0 = 0x11; q1 <<= 17; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                }
            }
            break;
        }
    } else if (area == (int)&_AREA_46) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 3:
        case 4:
        case 5:
        case 6:
            __WaitFrames(1);
            if (!__GetFlag(0x307)) {
                __Func_8092950(0xf, 0xf);
                p = __MapActor_GetActor(0xf);
                __Actor_SetSpriteFlags(p, 0);
                p = __MapActor_GetActor(0x13);
                __Actor_SetSpriteFlags(p, 0);
            }
            if (!__GetFlag(0xc2 << 2)) {
                __Func_8092950(0x10, 0xf);
                p = __MapActor_GetActor(0x10);
                __Actor_SetSpriteFlags(p, 0);
                p = __MapActor_GetActor(0x14);
                __Actor_SetSpriteFlags(p, 0);
            }
            if (!__GetFlag(0x309)) {
                __Func_8092950(0x11, 0xf);
                p = __MapActor_GetActor(0x11);
                __Actor_SetSpriteFlags(p, 0);
                p = __MapActor_GetActor(0x15);
                __Actor_SetSpriteFlags(p, 0);
            }
            break;
        case 7:
            p = __MapActor_GetActor(0xd);
            x = *(int *)(p + 8);
            p = __MapActor_GetActor(0xd);
            x >>= 20;
            OvlFunc_927_2008244(2, x, *(int *)(p + 0x10) >> 20, 1, 1, 0xff);
            __Func_8092950(0xd, 6);
            __WaitFrames(1);
            { unsigned char *r = __MapActor_GetActor(8); unsigned char b = 8; r[0x59] |= b; }
            OvlFunc_927_20088c0(8);
            break;
        case 8:
        case 9:
        case 0xa:
        case 0xb:
            y = 0xb9 << 17;
            OvlFunc_927_2008a4c(0x2de0000, 0, y, 0xdf);
            OvlFunc_927_2008a4c(0x2f20000, 0, y, 0xdf);
            OvlFunc_927_20088c0(0xa);
            OvlFunc_927_20088c0(0xc);
            if (!__GetFlag(0x312)) {
                OvlFunc_927_20088c0(9);
            } else {
                __WaitFrames(1);
                __MapActor_SetAnim(9, 4);
                __MapActor_SetPos(9, 0x2ba0000, 0xc7 << 17);
                { unsigned char *r = __MapActor_GetActor(9); unsigned char b = 2; r[0x23] |= b; }
                { int s3a = 0x2a; int s3b = 0x17;
                  __Func_8010704(0x1a, 0x14, 2, 4, s3a, s3b); }
                __SetFlag(0x85 << 2);
                { PIN3; q1 = 0x9e; q2 = 0xdc; q0 = 0xe; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                p = __MapActor_GetActor(0xe);
                __Actor_SetSpriteFlags(p, 0);
            }
            if (!__GetFlag(0x313)) {
                OvlFunc_927_20088c0(0xb);
            } else {
                __WaitFrames(1);
                __MapActor_SetAnim(0xb, 4);
                __MapActor_SetPos(0xb, 0x29a0000, 0x2260000);
                __MapActor_GetActor(0xb)[0x23] = 2;
                { int s4a = 0x28; int s4b = 0x20;
                  __Func_8010704(0x1a, 0x14, 2, 4, s4a, s4b); }
            }
            p = __MapActor_GetActor(0xe);
            x = *(int *)(p + 8);
            p = __MapActor_GetActor(0xe);
            x >>= 20;
            OvlFunc_927_2008244(2, x, *(int *)(p + 0x10) >> 20, 1, 1, 0xff);
            __Func_8092950(0xe, 6);
            __WaitFrames(1);
            { unsigned char *r = __MapActor_GetActor(9); unsigned char b = 8; r[0x59] |= b; }
            if (!__GetFlag(0x30b)) {
                __Func_8092950(0x12, 0xf);
                p = __MapActor_GetActor(0x12);
                __Actor_SetSpriteFlags(p, 0);
                if (__GetFlag(0x30a)) {
                    { PIN3; q1 = 0xba; q2 = 0xfc; q0 = 0x16; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                    { PIN3; q1 = 0xba; q2 = 0xfc; q0 = 0x12; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
                }
            }
            OvlFunc_927_2009c34();
            break;
        case 0xc:
        case 0xd:
            __MapActor_SetBehavior(0x12, gScript_927__0200b084);
            if (__GetFlag(0x893)) {
                if (__GetFlag(0x89e))
                    __SetFlag(0x88f);
            }
            break;
        case 0xf:
            __SetFlag(0x89e);
            break;
        }
    }
    return 0;
}
