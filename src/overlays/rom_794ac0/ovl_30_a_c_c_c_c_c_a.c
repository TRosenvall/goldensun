/* WHOLE-FILE CONVERSION of asm/overlays/rom_794ac0/ovl_30_a_c_c_c_c_c_a.s --
 * OvlFunc_899_200a758 is its only function, so no split and no linker change.
 * 410 instructions, 1096 bytes, 439 encodings and 99 relocations identical.
 *
 * A HImode CONSTANT STORE IS POOLED ON THUMB AND NO SPELLING OF THE LITERAL
 * ESCAPES IT.  `mov r3, #5 / strh` came out as `ldrh r3, .L+0`.  In arm.md
 * *thumb_movhi_insn lists `mn` (alternative 1, the `ldrh`) BEFORE `I`
 * (alternative 5, the `mov`), and the movhi expander's "sign extend a constant and
 * keep it in an SImode reg" branch is TARGET_ARM ONLY -- Thumb has no path from a
 * HImode const_int to a `mov`.
 *
 * An `int` local gives the right instruction but the wrong register (r5, because
 * it crosses the GetActor call).  A DEDICATED `short *` LOCAL PLUS AN `int` VALUE
 * LOCAL is exact:
 *
 *     r = (short *)(__MapActor_GetActor(0x18) + 0x64);
 *     hv = 5;  *r = hv;
 *
 * Measured: bare literal 2, `int` local only 8, `int` + pin to r3 5 (both movs
 * deleted), reusing the file's shared `unsigned char *p` 13-17, dedicated
 * `short *` ZERO.  THIS IS THE MIRROR OF BATCH 280'S POOLED-ZERO FINDING and
 * belongs beside it: there a pooled zero had to stay a BARE LITERAL because the
 * halfword store was what pulled the pool up; here a halfword store's constant
 * has to be ROUTED THROUGH TWO LOCALS because Thumb cannot spell it as an
 * immediate at all.  Same instruction pair, opposite cures, and the mode is what
 * distinguishes them.
 *
 * PINNING A SHORT-LIVED VALUE TO A CALL-CLOBBERED REGISTER FORCES ITS
 * MATERIALISATION AFTER THE CALL.  Plain `int` locals kept getting r5 because gcc
 * hoisted them above the preceding call:
 *
 *     { register int c __asm__("r3"); register int b __asm__("r2");
 *       b = p[0x59]; c = 4; c |= b; p[0x59] = c; }
 *
 * Worth 9 -> 2.  Related: the ROM SHARES ONE ZERO across a QImode and two SImode
 * stores, and routing the byte store through the same `int` local was worth 9 -> 6.
 *
 * Pins at a greedy-drop fixpoint, drops verified jointly: 10 of 15 kept.
 * No per-file Makefile override applies to this stem.
 */
typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    unsigned char pad1c4[0x2c0 - 0x1c4];
} GlobalState;

extern GlobalState gState;
extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __StartTask(void (*fn)(void), int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int a);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_899_200a71c(void);
extern void OvlFunc_899_200aba0(void);
extern void OvlFunc_899_200abf0(void);
extern void OvlFunc_899_200adb4(void);
extern void OvlFunc_899_200afd4(void);
extern void OvlFunc_899_200b6f8(void);
extern void OvlFunc_899_200c5cc(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c698(int a, int b, int c, int d);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

int OvlFunc_899_200a758(void)
{
    unsigned char *p;
    int s1;
    int s2;
    int z;
    short *r;
    int hv;
    int u1;
    int u2;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    switch (gState.f1c2) {
    case 5:
        z = 0;
        __MapActor_GetActor(8)[0x55] = z;
        *(int *)(__MapActor_GetActor(8) + 0xc) = z;
        *(int *)(__MapActor_GetActor(8) + 0x14) = z;
        break;
    case 0xa:
        if (__GetFlag(0x85 << 4)) {
            { PIN3; q0 = 2; q1 = 0xbc << 17; q2 = 0xbc << 17; __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q0 = 2; q1 = 0x80 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        }
        if (__GetFlag(0x856))
            __MapActor_SetPos(2, 0, 0);
        if (__GetFlag(0x855)) {
            { PIN3; q0 = 0x10; q1 = 0x86 << 18; q2 = 0xe8 << 17; __MapActor_SetPos(q0, q1, q2); }
            __MapActor_SetBehavior(0x10, 1);
            { PIN3; q0 = 0x10; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        }
        break;
    case 7:
    case 0xb:
        if (__GetFlag(0x855)) {
            { PIN3; q0 = 0x12; q1 = 0x8e << 18; q2 = 0xa2 << 18; __MapActor_SetPos(q0, q1, q2); }
            __MapActor_SetBehavior(0x12, 1);
            { PIN3; q0 = 0x12; q1 = 0x80 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
            OvlFunc_899_200c698(0xe7, (0x8e << 18), (0x80 << 13), (0xa8 << 18));
            __StartTask(OvlFunc_899_200a71c, 0xc8 << 4);
        } else if (__GetFlag(0x853)) {
            __MapActor_SetPos(0x12, 0, 0);
        }
        break;
    case 0xc:
        if (__GetFlag(0x109) && __GetFlag(0x852) && !__GetFlag(0x853)
            && __GetFlag(0xc0 << 2)) {
            u1 = 0xe;
            u2 = 0x2c;
            __Func_8010704(0xe, 0x2d, 3, 1, u1, u2);
            __StartTask(OvlFunc_899_200aba0, 0xc8 << 4);
            break;
        }
        if (__GetFlag(0x856)) {
            { PIN3; q0 = 0x19; q1 = 0xf0 << 15; q2 = 0xae << 18; __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q0 = 0x19; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        }
        if (!__GetFlag(0x852))
            break;
        s1 = 0xe;
        s2 = 0x2c;
        __Func_8010704(0xe, 0x2d, 3, 1, s1, s2);
        if (!__GetFlag(0x853)) {
            __Func_8010704(0xe, 0x32, 3, 1, s1, s2);
        } else {
            r = (short *)(__MapActor_GetActor(0x18) + 0x64);
            hv = 5;
            *r = hv;
            r = (short *)(__MapActor_GetActor(0x19) + 0x64);
            hv = 4;
            *r = hv;
            __StartTask(OvlFunc_899_200aba0, 0xc8 << 4);
        }
        break;
    case 0xd:
    case 0xe:
        __CutsceneWait(2);
        s1 = 2;
        s2 = 0xa;
        __CopyMapTiles(0x36, 2, 0x23, 0x14, s1, s2);
        __CopyMapTiles(0x36, 2, 0x5f, 0x14, s1, s2);
        __CopyMapTiles(0x36, 2, 0x23, 0x50, s1, s2);
        s1 = 4;
        s2 = 8;
        __CopyMapTiles(0x36, 2, 0x2e, 0x15, s1, s2);
        __CopyMapTiles(0x36, 2, 0x2e, 0x51, s1, s2);
        p = __MapActor_GetActor(0x1a);
        { register int c __asm__("r3"); register int b __asm__("r2");
          b = p[0x59]; c = 4; c |= b; p[0x59] = c; }
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x1a), 0);
        if (!__GetFlag(0x859))
            break;
        { PIN3; q0 = 0x1a; q1 = 0xa9 << 18; q2 = 0x19b0000; __MapActor_SetPos(q0, q1, q2); }
        u1 = 0x29;
        u2 = 0x18;
        __Func_8010704(0x65, 0x18, 3, 4, u1, u2);
        break;
    case 0xf:
        __CutsceneWait(2);
        s1 = 2;
        s2 = 8;
        __CopyMapTiles(0x36, 2, 0x2c, 0x15, s1, s2);
        __CopyMapTiles(0x36, 2, 0x2c, 0x51, s1, s2);
        __CutsceneStart();
        if (__GetFlag(0x855)) {
            __MapActor_SetAnim(0xf, 2);
            __MapActor_SetAnim(0x10, 2);
            __CutsceneWait(1);
            __MapActor_SetAnim(0x11, 2);
            break;
        }
        __MapActor_SetPos(8, 0xce << 18, 0xe4 << 17);
        if (__GetFlag(0x854)) {
            OvlFunc_899_200adb4();
            __CutsceneEnd();
            break;
        }
        __MapActor_SetAnim(8, 7);
        OvlFunc_899_200c5cc();
        __PlaySound(0x11);
        __MessageID(0x12c3);
        OvlFunc_899_200c5f4(8, 0xa);
        __CutsceneEnd();
        break;
    case 0x10:
        s1 = 0x2c;
        __Func_80105d4(0x36, 2, 2, 8, s1, 0x15);
        __Func_80105d4(0x36, 2, 2, 8, s1, 0x51);
        __MapActor_SetPos(8, 0xce << 18, 0xe4 << 17);
        OvlFunc_899_200abf0();
        __Func_8091e9c(0x10);
        break;
    case 0x11:
        s1 = 0x2c;
        __Func_80105d4(0x36, 2, 2, 8, s1, 0x15);
        __Func_80105d4(0x36, 2, 2, 8, s1, 0x51);
        if (!__GetFlag(0x109)) {
            { PIN3; q0 = 8; q1 = 0xce << 18; q2 = 0xe4 << 17; __MapActor_SetPos(q0, q1, q2); }
            OvlFunc_899_200afd4();
            OvlFunc_899_200b6f8();
        } else {
            __MapActor_SetAnim(0xf, 2);
            __MapActor_SetAnim(0x10, 2);
            __CutsceneWait(1);
            __MapActor_SetAnim(0x11, 2);
        }
        break;
    }
    return 0;
}
