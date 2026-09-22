/* OvlFunc_882_2008434 -- NON-MATCHING, 424 encodings of 592, size 1496 against the
 * ROM's 1500 (-4), 590 instructions against 592.
 *
 * Blocker class: ALLOCATION PRESSURE -- ONE EXTRA CALLEE-SAVED REGISTER.  This is
 * the only one of its agent's four targets still structurally wrong, and the
 * difference count is NOT a distance.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_77dd1c/2008434.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c_a.s
 * ONE function in the reference -- it CONVERTS WHOLE, no split.
 *
 * `push {r5, r6, r7, lr}` AGAINST THE ROM'S `push {r5, r6, lr}`.  The ROM holds
 * 0xe000 in r6, the behaviour-script pointer in r8, and the +0x64 address in R3;
 * ours puts the +0x64 address in r6 and pushes everything else up a register.  ONE
 * SHORT-LIVED ADDRESS PSEUDO IS TAKING A CALLEE-SAVED REGISTER THE ROM SPENDS ON A
 * CONSTANT.
 *
 * Its one relocation difference is NOT a defect: `__umodsi3` against
 * `_umodsi3_RAM`, which is the alias at overlays/rom_77dd1c/overlay.ld:103.
 *
 * MEASURED: five spellings of the +0x66 store; four placements of the
 * script-pointer local; sequencing the __Random() % 0x5a result through a temp first
 * (MUCH worse, 538); an 80-compile greedy barrier/fold ladder (69 -> 64 regions).
 *
 * HImode CONSTANT STORES CONFIRMED BOTH WAYS IN ONE FUNCTION -- another instance of
 * the rule that this class has no single direction.  The ROM stores 1,2,3,4 to
 * +0x66 with `mov r3,#N` and 0xe000 to +6 with `mov/lsl`.  The bare literal gives
 * `ldr r3,=1` from the pool (109 regions); routing through an `int` local gives the
 * ROM's `movs` (77).  BUT THE int LOCAL IS THEN HOISTED ABOVE THE STORE, taking r3
 * and pushing the +0x64 address into a callee-saved register -- a BARRIER after the
 * assignment recovered it (77 -> 69).
 *
 * ================================================================
 * THE MEASUREMENT RULE BIT HERE IN THE DIRECTION THAT MATTERS: THE NORMALISED COUNT
 * ORDERED CANDIDATES WRONGLY, NOT MERELY IMPRECISELY
 * ================================================================
 *
 *     variant                   tryc regions      objcmp encodings
 *     a434_v5 (ladder output)   64  (best)        433
 *     a434_b_D_bothinblock      76  (worst)       424  (best)
 *
 * Picking by the normalised count would have shipped the WORSE file.  Once the
 * instruction counts disagree (590 against 592 here) the normalised count is not
 * just imprecise -- ITS ORDERING IS WRONG.  Every subsequent ladder step was
 * re-scored with objcmp.  This is the third angle on batch 280's lesson, after the
 * pool case and the jump-table case.
 *
 * No .sym entry is implied -- 0x14b0000 and 0x47b0000 reproduce as plain literals
 * and neither has the in-function control the bar requires.  No ALIAS_CFLAGS row is
 * needed for this overlay: the known-open item for OvlFunc_882_200c41c did not
 * recur here.  No per-file Makefile flag override applies to this stem.
 *
 * NEXT: the extra callee-saved register is the whole function.  Find what makes the
 * +0x64 address short-lived enough to sit in r3, and the 424 should collapse.
 */
extern unsigned int gState;
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_882__0200c9f4[];
extern unsigned char gScript_882__0200cec8[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern unsigned int __Random(void);
extern void __StartRain(void);
extern void __StartThunder(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091ff0(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8095268(void);
extern void OvlFunc_882_200810c(void);
extern void OvlFunc_882_2008a10(void);
extern void OvlFunc_882_2008ec4(void);
extern void OvlFunc_882_20090a4(void);
extern void OvlFunc_882_20092f0(void);
extern void OvlFunc_882_2009498(void);
extern void OvlFunc_882_200c0f0(void);
extern void OvlFunc_882_200c5b8(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")
#define PINR1 register int q1 __asm__("r1")

int OvlFunc_882_2008434(void)
{
    int e5;
    int e6;
    int w;
    int n;
    unsigned char *beh;

    __Func_8091ff0(0xaa);
    __MapActor_SetPos(0x17, 0, 0);
    if (__GetFlag(0x109) != 0) {
        __ClearFlag(0x205);
        __ClearFlag(0x206);
    }
    if (__GetFlag(0x83 << 4) != 0) {
        { PIN3; q1 = 0xa7; q2 = 0xe9; q0 = 0xb; q1 <<= 17; q2 <<= 18;
          __MapActor_SetPos(q0, q1, q2); }
        OvlFunc_882_2008ec4();
    }
    if (__GetFlag(0x831) != 0) {
        { PIN3; q1 = 0xe0; q2 = 0xda; q0 = 0xc; q1 <<= 16; q2 <<= 18;
          __MapActor_SetPos(q0, q1, q2); }
        OvlFunc_882_20090a4();
    }
    if (__GetFlag(0x832) != 0) {
        { PIN3; q1 = 0x80; q0 = 0xd; q1 <<= 15; q2 = 0x2bf0000;
          __MapActor_SetPos(q0, q1, q2); }
        OvlFunc_882_20092f0();
    }
    if (__GetFlag(0x833) != 0) {
        { PIN3; q1 = 0xd8; q0 = 0xe; q1 <<= 17; q2 = 0x47b0000;
          __MapActor_SetPos(q0, q1, q2); }
        OvlFunc_882_2009498();
    }
    *(__MapActor_GetActor(0xb) + 0x59) |= 4;
    *(__MapActor_GetActor(0xc) + 0x59) |= 4;
    *(__MapActor_GetActor(0xd) + 0x59) |= 4;
    *(__MapActor_GetActor(0xe) + 0x59) |= 4;
    *(__MapActor_GetActor(0xf) + 0x59) |= 4;
    *(__MapActor_GetActor(0x10) + 0x59) |= 4;
    *(__MapActor_GetActor(0x11) + 0x59) |= 4;
    *(__MapActor_GetActor(0x12) + 0x59) |= 4;
    if (__GetFlag(0x837) != 0)
        __MapActor_SetPos(0x16, 0, 0);
    { unsigned char *q = __MapActor_GetActor(0x13);
      *(int *)(q + 0x18) = 0x80 << 10;
      *(int *)(q + 0x1c) = 0x80 << 10; }
    if (__GetFlag(0x838) != 0) {
        { PIN3; q1 = 0xe4; q0 = 0x13; q1 <<= 15; q2 = 0x14d0000;
          __MapActor_SetPos(q0, q1, q2); }
    } else {
        __MapActor_SetBehavior(0x13, gScript_882__0200c9f4);
    }
    if (__GetFlag(0x841) != 0) {
        OvlFunc_882_200c0f0();
        { PIN3; q1 = 0xa5; q2 = 0x4cd0000; q1 <<= 16; q0 = 9;
          __MapActor_SetPos(q0, q1, q2); }
        { unsigned char *q = __MapActor_GetActor(9);
          w = 0xe0 << 8;
          *(unsigned short *)(q + 6) = w;
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c;
          beh = gScript_882__0200cec8;
          n = 1; __asm__ volatile ("" : : "r" (n));
          *(unsigned short *)(q + 0x66) = n; }
        __MapActor_SetBehavior(9, beh);
        { PIN3; q1 = 0xa5; q2 = 0x4e60000; q1 <<= 16; q0 = 0x1a;
          __MapActor_SetPos(q0, q1, q2); }
        { unsigned char *q = __MapActor_GetActor(0x1a);
          *(unsigned short *)(q + 6) = w;
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c;
          n = 2; __asm__ volatile ("" : : "r" (n));
          *(unsigned short *)(q + 0x66) = n; }
        __MapActor_SetBehavior(0x1a, beh);
        { PIN3; q1 = 0x98; q2 = 0x5050000; q1 <<= 16; q0 = 0x16;
          __MapActor_SetPos(q0, q1, q2); }
        { unsigned char *q = __MapActor_GetActor(0x16);
          *(unsigned short *)(q + 6) = w;
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c;
          n = 3; __asm__ volatile ("" : : "r" (n));
          *(unsigned short *)(q + 0x66) = n; }
        __MapActor_SetBehavior(0x16, beh);
        { PIN3; q1 = 0xb8; q2 = 0xa3; q2 <<= 19; q1 <<= 16; q0 = 8;
          __MapActor_SetPos(q0, q1, q2); }
        { unsigned char *q = __MapActor_GetActor(8);
          *(unsigned short *)(q + 6) = w;
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c;
          n = 4; __asm__ volatile ("" : : "r" (n));
          *(unsigned short *)(q + 0x66) = n; }
        __MapActor_SetBehavior(8, beh);
        { PIN2; q1 = 6; q0 = 8; __MapActor_SetAnim(q0, q1); }
        *(__MapActor_GetActor(0x16) + 0x23) &= 0xfe;
        *(__MapActor_GetActor(8) + 0x23) &= 0xfe;
        { PINR1; q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_882_200c5b8, q1); }
        __MapActor_SetPos(0x18, 0, 0);
        __MapActor_SetPos(0x19, 0, 0);
        __MapActor_SetPos(0x17, 0, 0);
        __MapActor_SetPos(0x13, 0, 0);
        if (__GetFlag(0x842) != 0)
            __MapActor_SetPos(0x16, 0, 0);
    } else if (__GetFlag(0x83a) != 0) {
        { PIN3; q1 = 0xc0; q0 = 0xa; q1 <<= 16; q2 = 0x4be0000;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0xa; q1 <<= 6; __Func_8092adc(q0, q1, q2); }
        { PIN2; q1 = 5; q0 = 0xa; __MapActor_SetAnim(q0, q1); }
        { unsigned char *q = __MapActor_GetActor(0xa);
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c;
          beh = gScript_882__0200cec8; }
        __MapActor_SetBehavior(0xa, beh);
        { PIN3; q1 = 0xe3; q0 = 0x18; q1 <<= 16; q2 = 0x4be0000;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0x18; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
        { PIN2; q1 = 6; q0 = 0x18; __MapActor_SetAnim(q0, q1); }
        { unsigned char *q = __MapActor_GetActor(0x18);
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c; }
        __MapActor_SetBehavior(0x18, beh);
        { PIN3; q1 = 0xf7; q0 = 0x19; q1 <<= 16; q2 = 0x4be0000;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0x19; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
        { PIN2; q1 = 6; q0 = 0x19; __MapActor_SetAnim(q0, q1); }
        { unsigned char *q = __MapActor_GetActor(0x19);
          *(unsigned short *)(q + 0x64) = (__Random() % 0x5a) + 0x3c; }
        __MapActor_SetBehavior(0x19, beh);
        { PIN3; q1 = 0xf3; q0 = 0x17; q1 <<= 16; q2 = 0x4fd0000;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q2 = 0; q1 <<= 8; q0 = 0x17; __Func_8092adc(q0, q1, q2); }
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
        __MapActor_SetPos(0x11, 0, 0);
        __MapActor_SetPos(0x12, 0, 0);
    } else {
        __MapActor_SetPos(0x11, 0, 0);
        __MapActor_SetPos(0x12, 0, 0);
    }
    if (*(short *)((char *)&gState + (0xe1 << 1)) != 0xf || __GetFlag(0x87b) != 0) {
        __StartRain();
        __StartThunder();
    }
    if (__GetFlag(0x84 << 2) != 0)
        OvlFunc_882_200810c();
    __SetFlag(0x834);
    e6 = 0x2e;
    __Func_8010704(0x1d, 0x18, 1, 2, 0x1a, e6);
    __Func_8010704(0x1d, 0x19, 1, 1, 0x1b, e6);
    __Func_8010704(0x1d, 0x19, 1, 1, 0x1c, e6);
    e5 = 0x14;
    __Func_8010704(0x13, 0x5a, 1, 1, e5, 0x58);
    __Func_8010704(0x13, 0x5a, 1, 1, e5, 0x59);
    { unsigned char *q = __MapActor_GetActor(0x15);
      q[0x55] = 0;
      *(int *)(q + 0xc) = 0xc0 << 16;
      q[0x59] = 8;
      __Actor_SetSpriteFlags(q, 0); }
    __WaitFrames(1);
    if (__GetFlag(0x87b) == 0
        && *(short *)((char *)&gState + (0xe1 << 1)) == 0xf) {
        OvlFunc_882_2008a10();
    } else {
        __MapActor_SetAnim(0x17, 7);
        if (__GetFlag(0x837) == 0) {
            __CutsceneStart();
            __MapActor_Surprise(0x16, 0x101);
            { PIN3; q1 = 0xc8; q0 = 0x16; q1 <<= 17; q2 = 0x2630000;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q1 = 0xd4; q0 = 0x15; q1 <<= 17; q2 = 0x2730000;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q1 = 0xc8; q0 = 0x16; q1 <<= 1; q2 = 0x26b;
              __Func_809218c(q0, q1, q2); }
            { PIN3; q1 = 0xd4; q2 = 0x26b; q0 = 0x15; q1 <<= 1;
              __Func_80921c4(q0, q1, q2); }
            __MapActor_SetAnim(0x15, 2);
            __MapActor_SetAnim(0x16, 5);
            __CutsceneEnd();
        } else {
            __CutsceneStart();
            { PIN3; q1 = 0xd4; q0 = 0x15; q1 <<= 17; q2 = 0x2730000;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q1 = 0xd4; q0 = 0x15; q1 <<= 1; q2 = 0x26b;
              __Func_80921c4(q0, q1, q2); }
            __MapActor_SetAnim(0x15, 3);
            __CutsceneEnd();
        }
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
        *(int *)(iwram_3001ebc + 0x1c8) = 0x18;
        __MapTransitionIn();
        __WaitMapTransition();
        __Func_8095268();
    }
    return 0;
}
