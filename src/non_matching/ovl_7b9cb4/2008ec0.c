/* OvlFunc_932_2008ec0 -- NON-MATCHING, 390 encodings of 497, size 1264 against the
 * ROM's 1240 (+24), 504 instructions against 497.  466 instructions in the reference.
 * THE LEAST-ADVANCED PARK OF BATCH 282 and honestly so -- it ran out of iteration
 * budget, not out of levers.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7b9cb4/2008ec0.c \
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a_c.s
 * ONE function, no data section -- CONVERTS WHOLE when it lands.
 *
 * Transcribed in full and taken 451 -> 390 by one pass of pin/split blocks at ~31
 * argument-fill sites.  THAT PASS ALSO FIXED THE STACK FRAME AS A SIDE EFFECT: the
 * first candidate emitted `sub sp, #0x10` with a duplicated `str r0, [sp,#8]`, and
 * the second emits the ROM's `sub sp, sp, #12` / `str r0, [sp, #8]`.
 *
 * TWO FACTS WORTH CARRYING FORWARD:
 *
 * THE EPILOGUE IS `pop {r0} / bx r0`, NOT `pop {r1} / bx r1`, SO THIS FUNCTION
 * RETURNS void.  By the thumb_exit reading -- regs_available_for_popping comes from
 * GET_MODE_SIZE (DECL_MODE (DECL_RESULT (decl))), VOIDmode giving r0|r1|r2 and
 * picking r0 -- `void OvlFunc_932_2008ec0(int wait)` is the signature, and the
 * prologue and epilogue matched EXACTLY on the first compile.  This is the same tell
 * that batch 281 used in the other direction (a missing `mov r0, #0` meaning the
 * function DOES return a value); both polarities now have an instance.
 *
 * THE ROM HAS ITS OWN `.pool_aligned` MID-FUNCTION DUMP before .L1388, so unlike its
 * two batch-282 siblings a pool jump here is CORRECT and must not be optimised away.
 * Do not apply the int-carrier lever blindly across a bank -- read where each ROM's
 * pool actually is.
 *
 * The remaining 504-vs-497 excess and the residue are pairwise register and order
 * swaps at the `a9` field-store cluster and the `mov r11, r0` zero-carrier.
 *
 * tryc's `!!` pool warning fires on this reference, so every number here is objcmp.
 * No per-file Makefile flag override applies to this stem.
 *
 * NEXT: another full ladder pass.  This is a budget park, not a blocked one.
 */
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

extern unsigned char gScript_932__0200bd78[];
extern unsigned char gScript_932__0200bdec[];

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern int __StartTask(void (*fn)(void), int n);
extern void __StopTask(void (*fn)(void));
extern void OvlFunc_932_2008098(void);
extern void OvlFunc_932_200abe0(void);

void OvlFunc_932_2008ec0(int wait)
{
    unsigned char *a0;
    unsigned char *a8;
    unsigned char *a9;
    unsigned char *aa;
    unsigned char *t;
    short *f0;
    short *f8;
    int zero;
    int v312;
    int sp80;
    int k;
    int neg;

    a0 = __MapActor_GetActor(0);
    a8 = __MapActor_GetActor(8);
    a9 = __MapActor_GetActor(9);
    aa = __MapActor_GetActor(0xa);
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    { PIN2; q0 = 0x80; q1 = 0x80; q0 <<= 9; q1 <<= 6; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xc4; q1 = 1; q2 = 0xe8; q3 = 1; q0 <<= 18; q1 = -q1; q2 <<= 15;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q1 = 0x80; q2 = 0x80; q2 <<= 9; q0 = 0; q1 <<= 10;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0, 6);
    { PIN3; q1 = 0xc6; q2 = 0x8c; q0 = 0; q1 <<= 2; __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0x64; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x101; q2 = 0x3c; q0 = 0; __MapActor_Emote(q0, q1, q2); }
    __PlaySound(0xb7);
    { PIN3; q0 = 0xc0; q1 = 0xc0; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 <<= 10;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    *(int *)(a9 + 0x18) = 0x13333;
    *(int *)(a9 + 0x1c) = 0x13333;
    a9[0x23] |= 2;
    *(void **)(a9 + 0x6c) = (void *)OvlFunc_932_2008098;
    zero = 0;
    __MapActor_SetAnim(8, 4);
    *(int *)(a8 + 0x44) = 0x80 << 8;
    v312 = 0x3120000;
    *(int *)(a8 + 8) = v312;
    *(int *)(a8 + 0xc) = 0x80 << 14;
    *(int *)(a8 + 0x10) = 0xb4 << 15;
    sp80 = 0x80 << 10;
    *(int *)(a8 + 0x18) = sp80;
    *(int *)(a8 + 0x1c) = sp80;
    __CutsceneWait(0xa);
    __PlaySound(0xb7);
    { PIN3; q0 = 0x80; q2 = 0x80; q1 = sp80; q2 <<= 9; q0 <<= 11;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    *(int *)(aa + 8) += 0xe0 << 12;
    *(int *)(aa + 0xc) += 0xfff80000;
    *(short *)(*(unsigned char **)(aa + 0x50) + 0x1e) = 0xc0 << 8;
    __PlaySound(0x6b);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q1 = 0x81; q1 <<= 1; q2 = 0x50; q0 = 0; __MapActor_Emote(q0, q1, q2); }
    __PlaySound(0x37);
    { PIN3; q0 = 0x80; q1 = 0xc0; q2 = 0x80; q2 <<= 9; q0 <<= 9; q1 <<= 10;
      __Func_8012330(q0, q1, q2); }
    __Func_8092b08(8, 0);
    __Func_8092b08(0, 0);
    { PIN2; q0 = 0; q1 = 0x101; __MapActor_Surprise(q0, q1); }
    { PIN3; q1 = 0xa0; q2 = 0xa0; q0 = 0; q1 <<= 10; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    f0 = (short *)(a0 + 0x64);
    *f0 = zero;
    __MapActor_SetBehavior(0, gScript_932__0200bdec);
    if (__GetFlag(0x205) != 0) {
        { PIN3; q2 = 0x84; q0 = 1; q1 = 0x36e0000; q2 <<= 18;
          __MapActor_SetPos(q0, q1, q2); }
        *(short *)(__MapActor_GetActor(1) + 6) = 0xa0 << 7;
    }
    { PIN2; q0 = 0xa0; q1 = 0xa0; q0 <<= 9; q1 <<= 6; __Func_80933d4(q0, q1); }
    { PIN4; q1 = 1; q2 = 0x8b; q2 <<= 18; q3 = 1; q1 = -q1; q0 = v312;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneWait(wait);
    __Func_8092b08(8, 1);
    __MapActor_SetSpeed(8, 0x195c2, 0xcae1);
    f8 = (short *)(a8 + 0x64);
    *f8 = zero;
    __MapActor_SetBehavior(8, gScript_932__0200bd78);
    do {
        __WaitFrames(1);
    } while (*f0 == 0);
    __MapActor_Surprise(0, 0);
    do {
        __WaitFrames(1);
    } while (*f8 == 0);
    __Func_8092b08(0, 2);
    __MapActor_GetActor(0)[0x23] |= 1;
    __PlaySound(0x121);
    { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    *(int *)(a9 + 8) = 0x3120000;
    neg = 0xffc00000;
    *(int *)(a9 + 0x6c) = 0;
    *(int *)(a9 + 0x10) = 0x26a0000;
    *(int *)(a9 + 0xc) = neg;
    __MapActor_SetSpeed(8, 0x19999, 0xcccc);
    *(int *)(a8 + 0x44) = 0x1999;
    *(int *)(a8 + 0x48) = 0x3333;
    k = 0x80 << 11;
    *(int *)(a8 + 0x28) = k;
    { PIN3; q2 = 0x97; q0 = 8; q1 = 0x312; q2 <<= 2; __Func_8092158(q0, q1, q2); }
    __MapActor_SetSpeed(8, 0x33333, 0x19999);
    { PIN3; q2 = 0xa1; q1 = 0x312; q2 <<= 2; q0 = 8; __MapActor_TravelTo(q0, q1, q2); }
    __CutsceneWait(0xf);
    { PIN3; q0 = 0xa0; q1 = 0xe0; q2 = 0x80; q0 <<= 11; q1 <<= 11; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    __CopyMapTiles(0x19, 0x24, 0x2b, 0x24, 0xb, 9);
    __Func_8010704(0x19, 0x23, 0xa, 5, 0x2b, 0x23);
    __MapActor_SetPos(8, 0, 0);
    __MapActor_SetPos(9, 0, 0);
    __StartTask(OvlFunc_932_200abe0, 0xc8 << 4);
    __CutsceneWait(0x50);
    __StopTask(OvlFunc_932_200abe0);
    __CutsceneWait(0x3c);
    __PlaySound(0x11);
    { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x78);
    if (__GetFlag(0x205) != 0) {
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 9; q2 <<= 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0xce; q0 = 1; q1 <<= 2; q2 = 0x22e; __Func_809218c(q0, q1, q2); }
    }
    __MapActor_SetSpeed(0, 0x9999, 0x4ccc);
    { PIN3; q2 = 0x92; q0 = 0; q1 = 0x356; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    if (__GetFlag(0x205) != 0) {
        __MapActor_SetAnim(1, 1);
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    }
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = 1; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = 0; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN4; q0 = 0xc5; q1 = neg; q2 = 0x2620000; q3 = 1; q0 <<= 18;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __PlaySound(0x94);
    __CutsceneWait(0xf0);
    if (__GetFlag(0x205) != 0) {
        { PIN2; q1 = 0x80; q0 = k; q1 <<= 8; __Func_80933d4(q0, q1); }
        { PIN4; q2 = 0x92; q3 = 1; q0 = 0x3560000; q1 = 0; q2 <<= 18;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        { PIN3; q1 = 0xd2; q2 = 0x8a; q0 = 1; q1 <<= 2; q2 <<= 2;
          __Func_80921c4(q0, q1, q2); }
        __Func_80921c4(1, 0x356, 0x232);
        __MapActor_SetAnim(1, 2);
        t = __MapActor_GetActor(0);
        if (t != 0)
            __MapActor_TravelTo(1, *(short *)(t + 0xa), *(short *)(t + 0x12));
        __MapActor_WaitMovement(1);
        __MapActor_SetPos(1, 0, 0);
    }
    __PlayMapMusic();
    __SetFlag(0x908);
}
