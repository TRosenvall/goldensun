// fakematch
/* ovl_30_c_c_c_c_c_c_c_c_c_c_c_a.c  --  OvlFunc_909_20099b0
 *   [the WHOLE of asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c_c_a.s --
 *    ONE `.thumb_func_start`, so NO SPLIT is required.  overlay.ld carries TWO
 *    lines for this object, 45 `(.text)` and 58 `(.data)`, and BOTH stay
 *    VERBATIM -- the .s has no .data today and the compiled .c has none either,
 *    so line 58 contributes an empty section before and after.]
 *
 *   OK OvlFunc_909_20099b0 -- 2060 bytes, 807 encodings and 191 relocations
 *      identical.  Re-measured thirteen times.
 *
 * objcmp prints no `(built with: ...)` line: adjust = set(), the tree default.
 * `makefile_flags()` is the EMPTY set; the two literal rom_79c738 rules in the
 * Makefile name other stems, so `asm/%.o: src/%.c` fires.  No
 * .section/.data/.bss/.word/.byte in the .s; all 23 `.L` symbols are branch
 * targets DEFINED IN THIS FILE.  FAKEMATCH: register-pin idiom.
 *
 * A cutscene: ~769 instructions, four message bases, and six repetitions of a
 * save-bit idiom that needs a FLAG LOCAL in the source.
 *
 * THE `int f` FLAG IS REAL SOURCE, NOT AN ARTEFACT.  Six times the ROM does
 * `ldr r0, =0x84f / mov r5, #1 / bl __GetFlag / cmp r0, #0 / bne L / <bump> /
 * mov r5, #0 / L: <call> / cmp r5, #0 / beq M / <bump> / M:`.  That is
 * `f = 1; if (__GetFlag(0x84f) == 0) { bump(); f = 0; } call(); if (f != 0)
 * bump();` -- the same counter bumped BEFORE the call when the bit is clear and
 * AFTER it when the bit is set.  r5 is the flag; r6 is &iwram_3001ebc, held
 * across the first half and reloaded in the second, which gcc reproduces
 * unaided.
 *
 * hi = 0, hiv = 0 AND AGAIN GCC SPENDS FOUR HIGH REGISTERS.  Plain C is 754 of
 * 807 differing, +40 bytes, first differing encoding the PUSH MASK
 * (`push {r5, r6, r7, lr}` against the ROM's `push {r5, r6, lr}`).  The
 * commoning pool is -1, 0x37e0000, 0x31e0000, 0x9999, 0x4ccc, 0x84f and
 * &iwram_3001ebc where the ROM holds only the flag and the iwram address.
 *
 * WHAT CLOSED IT.  The first three rows are objcmp's encodings-differing count
 * (pool-offset dominated); the last four are pool-normalised and agree with
 * objcmp from the exact-size row onward:
 *
 *   plain C                                                    754  (+40 bytes)
 *   + every pinnable site pinned                                93  (+24 bytes)
 *   + the six __GetFlag(0x84f) conditions pinned too             79  (+16 bytes)
 *   + GetActor/SetPos and GetActor/TravelTo with r0 LAST         11  (exact size)
 *   + the __MapActor_SetBehavior(0x13, script) fill reversed      9
 *   + the __Func_8093554 byte store's zero born BEFORE the        4
 *     first call                                                    (relocs still differ)
 *   + q2 nominated FIRST at __Func_8092a1c                        0
 *
 * A CONDITION IS A PIN SITE.  The 0x84f is the hottest commoning candidate in
 * the function and every one of its six occurrences is inside an `if (...)`, so
 * no statement-level pin can reach it.  A STATEMENT EXPRESSION does:
 * `if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) == 0)`.  Worth 93 -> 79 as a set.
 * NEW as a mechanic -- the recorded pin idiom is always shown on a statement --
 * and it is the only way to evict a constant whose only uses are in tests.
 *
 * THE BYTE STORE'S ZERO IS A PUSHED PSEUDO.  `b = __Func_8093554() + 0x55;
 * *b = 0;` gives `movs r3, #0 / ... / strb r3, [r0]` and then rebuilds r3 for
 * the next call's fourth argument; the ROM has `movs r5, #0 / ... / strb r5`,
 * i.e. the stored zero in a CALLEE-SAVED register.  This is the recorded rule
 * "a caller-saved register means the pseudo is born after the last call, a
 * pushed callee-saved one means it is born before the first" -- and the cure is
 * the recorded one: an `int z; z = 0;` at the top of the function, `*b = z;`.
 * Worth 9 -> 4.  Reusing the existing `int f` for the same purpose
 * (`f = 0; ... *b = f;`) is EXACTLY TIED, which is what confirms the class: any
 * pseudo born before the first call will do.  CHECKED AGAINST THE DOCS FIRST --
 * this is a third instance of a recorded class, not a new one.
 *
 * THE LAST FIVE ENCODINGS WERE A POOL-ORDER TELL AND THE RECORDED CALLEE RULE
 * FIXED THEM.  At 4 differing the relocation line was still DIFFERING, with
 * exactly one entry moved: gOvl_0200a5c0 at 0x7e8 in the ROM and 0x7ec in ours.
 * The ROM emits `ldr r2, =gOvl / mov r0, #0 / ldr r1, =0x10013`; ours emitted
 * the two pooled loads the other way round, so gcc's `.word` list came out
 * rotated.  `{ PIN3; q2 = (int)gOvl; q0 = 0; f(q0, 0x10013, (void *)q2); }` --
 * the pooled pointer NOMINATED FIRST -- is exact.  That is the
 * ovl_30_c_a_c_c_c_a_a.c rule "ONE CALLEE, ONE FILL: the pooled script pointer
 * has to be nominated before the two `mov`s or sched2 sinks its `ldr` past
 * them", on the SAME callee __Func_8092a1c, now at a third site.  Measured in
 * the minimised base: q2-first is exact, ascending and NO PIN are both 5 with
 * relocations differing, and the `(int)` / `(void *)` casts are INERT -- the
 * ordering is the whole lever.
 *
 * THE RELOCATION LINE PARTITIONED THE RESIDUE EXACTLY, AGAIN.  Every candidate
 * with relocations DIFFERING had a pool-order defect; the one ordering residue
 * that survived with relocations SILENT was a single `movs r0, #0` transposition
 * and it went away with the same pin.  Worth trusting as the first thing to
 * read off an objcmp run.
 *
 * MEASURED WORSE / INERT (against 807 encodings / 2060 bytes):
 *
 *   spelling                                            differing
 *   -------------------------------------------------  ---------
 *   plain C, no pins                                         754  (+40 bytes)
 *   every pinnable site pinned                                93  (+24 bytes)
 *   ... + the six __GetFlag conditions                        79  (+16 bytes)
 *   GetActor/SetPos with r0 seeded FIRST instead of last       27
 *   the byte store sunk inside the next call's pin scope       12  (+2 insns)
 *   the byte store indexed `b[0x55]` / dereferenced inline     11 (INERT)
 *   __Func_8092a1c with no pin / ascending                      5  (RELOCS differ)
 *   __Func_8092a1c q1-first (120) / q0-first (012)              5 / 5
 *   do { } while (0) around __Func_8092a1c                      6
 *   INERT: __asm__ volatile("") at five places; all twelve
 *     fill orders at __Func_80921c4(0, 0x37e, 0xbf << 2);
 *     the 0x201/0x10 iwram stores written three other ways;
 *     22 of the 74 pins, dropped as one set, then 49 of
 *     the 52 survivors narrowed by width to a fixpoint.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_800fe9c(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, void *s);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);
extern void OvlFunc_909_2009958(void);
extern void OvlFunc_909_2009984(void);
extern unsigned char gScript_909__0200a5d4[];
extern unsigned char gOvl_0200a5c0[];
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_909_20099b0(void)
{
    unsigned char *a;
    unsigned char *b;
    int f;
    int z;

    z = 0;

    __CutsceneStart();
    { PIN3; q0 = -1; q1 = -1; q2 = -1; __Func_80933f8(q0, q1, q2, 0); }  /*0*/
    __WaitFrames(1);
    b = __Func_8093554() + 0x55;
    *b = z;
    { PIN3; q0 = 0x37e0000; q1 = -1; q2 = 0xa6 << 18; __Func_80933f8(q0, q1, q2, 0); }  /*1*/
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x13; q1 = 0xde << 18; q2 = 0x31e0000; __MapActor_SetPos(q0, q1, q2); }  /*2*/
    { PIN1; q0 = 0; __MapActor_SetPos(q0, 0xe2 << 18, 0x31e0000); }  /*3*/
    { PIN1; q0 = 0x9999; __Func_80933d4(q0, 0x1333); }  /*4*/
    { PIN2; q0 = 0x37e0000; q1 = -1; __Func_80933f8(q0, q1, 0x2ba0000, 1); }  /*5*/
    { PIN2; q0 = 0x13; q1 = 0xcccc; __MapActor_SetSpeed(q0, q1, 0x6666); }  /*6*/
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }  /*7*/
    { PIN2; q0 = 0x13; q1 = 0xde << 2; __Func_809218c(q0, q1, 0xb4 << 2); }  /*8*/
    __Func_809218c(0, 0xe2 << 2, 0xb8 << 2);
    __CutsceneWait(0x3c);
    __MapActor_WaitMovement(0x13);
    __MapActor_SetAnim(0x13, 1);
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x14);
    __Func_80925cc(0x13, 2);
    __MessageID(0x1728);
    f = 1;
    if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) == 0) {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        f = 0;
    }
    __ActorMessage(0x13, 0);
    if (f != 0)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    __Func_80933f8(0x37e0000, -1, 0xa6 << 18, 1);
    __MapActor_SetBehavior(0x13, gScript_909__0200a5d4);
    __Func_80921c4(0, 0x37e, 0xab << 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(int *)(a + 8); q2 = *(int *)(a + 0x10); q0 = 1;
          __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(int *)(a + 8); q2 = *(int *)(a + 0x10); q0 = 2;
          __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(int *)(a + 8); q2 = *(int *)(a + 0x10); q0 = 3;
          __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }  /*17*/
    { PIN2; q0 = 2; q1 = 0x9999; __MapActor_SetSpeed(q0, q1, 0x4ccc); }  /*18*/
    { PIN2; q0 = 3; q1 = 0x80 << 9; __MapActor_SetSpeed(q0, q1, 0x80 << 8); }  /*19*/
    __MapActor_SetAnim(1, 2);
    __MapActor_SetAnim(2, 2);
    __MapActor_SetAnim(3, 2);
    { PIN1; q0 = 1; __Func_809228c(q0, -0x10, 0x10); }  /*20*/
    __Func_809228c(2, 0x10, 0x10);
    __Func_809228c(3, 0x20, 0x10);
    __MapActor_WaitMovement(2);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    __MapActor_SetAnim(3, 1);
    __CutsceneWait(0xa);
    { PIN2; q0 = 1; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0); }  /*21*/
    __Func_8092adc(2, 0xc0 << 8, 0);
    __MapActor_WaitMovement(3);
    __Func_8092adc(3, 0xa0 << 8, 0);
    __MapActor_WaitScript(0x13);
    __CutsceneWait(0x14);
    f = 1;
    if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) == 0) {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        f = 0;
    }
    __Func_80925cc(0x12, 3);
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0x14); }  /*24*/
    if (f != 0)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    f = 1;
    if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) == 0) {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        f = 0;
    }
    __Func_80925cc(0x12, 1);
    __Func_8093040(0x2012, 0, 0xa);
    if (f != 0)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    OvlFunc_909_2009958();
    __CutsceneWait(0x14);
    if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) != 0) {
        { PIN1; q0 = 0; __MapActor_Emote(q0, 0x81 << 1, 0x3c); }  /*26*/
        { PIN1; q0 = 1; __MapActor_Emote(q0, 0x105, 0x28); }  /*27*/
    } else {
        __CutsceneWait(0x28);
    }
    { PIN2; q0 = 1; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0xa); }  /*28*/
    __Func_8093040(0x4001, 0, 0xa);
    { PIN1; q0 = 2; __Func_8092adc(q0, 0xc0 << 8, 0xa); }  /*30*/
    __MapActor_DoAnim(2, 3);
    __ActorMessage(0x4002, 0);
    { PIN1; q0 = 3; __Func_8092adc(q0, 0xa0 << 8, 0xa); }  /*32*/
    __MapActor_DoAnim(3, 3);
    __Func_8093040(0x4003, 0, 0x14);
    __MapActor_DoAnim(0x12, 3);
    __CutsceneWait(0x14);
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0x14); }  /*34*/
    { PIN1; q0 = 1; __MapActor_Emote(q0, 0x103, 0); }  /*35*/
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, 0x3c); }  /*36*/
    if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) != 0) {
        __Func_80925cc(0x12, 1);
        __MapActor_DoAnim(0x12, 4);
        { PIN1; q0 = 0x2012; __Func_8092c40(q0, 0); }  /*37*/
        OvlFunc_909_2009958();
        f = 1;
        if (__Func_8091c7c(0, 0) != 0) {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            f = 0;
        }
        __Func_8092adc(0x12, 0xa0 << 7, 0);
        OvlFunc_909_2009984();
        __CutsceneWait(0xa);
        __Func_8093040(0x2012, 0, 0xa);
        if (f != 0)
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        { PIN1; q0 = 0x12; __MapActor_Emote(q0, 0x81 << 1, 0x3c); }  /*40*/
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 4;
    }
    __Func_8092c40(0x2012, 0);
    OvlFunc_909_2009958();
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID(0x1737);
    else
        __MessageID(0x1738);
    OvlFunc_909_2009984();
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0x14); }  /*44*/
    __Func_80925cc(0x13, 1);
    __MessageID(0x1739);
    __Func_8093040(0x13, 0, 0xa);
    { PIN1; q0 = 0; __Func_8092adc(q0, 0xc0 << 7, 0); }  /*46*/
    { PIN1; q0 = 1; __Func_8092adc(q0, 0xe0 << 8, 0); }  /*47*/
    __Func_8092adc(2, 0, 0);
    { PIN1; q0 = 3; __Func_8092adc(q0, 0x80 << 8, 0x28); }  /*48*/
    __Func_80925cc(0x12, 2);
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*49*/
    __Func_8092adc(0, 0xc0 << 8, 0);
    OvlFunc_909_2009984();
    __CutsceneWait(0xa);
    { PIN1; q0 = 0x12; __MapActor_Emote(q0, 0x105, 0x3c); }  /*51*/
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*52*/
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x12, 3);
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*53*/
    { PIN1; q0 = 0x12; __MapActor_Emote(q0, 0x84 << 1, 0x3c); }  /*54*/
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*55*/
    { PIN2; q0 = 0x12; q1 = 0xc0 << 6; __Func_8092adc(q0, q1, 0xa); }  /*56*/
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*57*/
    __MapActor_DoAnim(0x12, 3);
    { PIN1; q0 = 0x2012; __Func_8092c40(q0, 0); }  /*58*/
    OvlFunc_909_2009958();
    f = 1;
    if (__Func_8091c7c(0, 0) == 1) {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        f = 0;
    }
    OvlFunc_909_2009984();
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*59*/
    if (f != 0)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN1; q0 = 0x12; __Func_8092adc(q0, 0xe0 << 7, 0xa); }  /*60*/
    __Func_80925cc(0x13, 1);
    { PIN2; q0 = 0x13; q1 = 0x80 << 5; __Func_8092adc(q0, q1, 0x14); }  /*61*/
    __MapActor_DoAnim(0x12, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x13, 3);
    __Func_8093040(0x13, 0, 0xa);
    { PIN2; q0 = 0x13; q1 = 0xc0 << 6; __Func_8092adc(q0, q1, 0xa); }  /*62*/
    { PIN1; q0 = 0x12; __Func_8092adc(q0, 0xc0 << 6, 0x14); }  /*63*/
    __Func_80925cc(0x12, 1);
    { PIN1; q0 = 0x2012; __Func_8093040(q0, 0, 0xa); }  /*64*/
    __MapActor_DoAnim(0x12, 3);
    __Func_8093040(0x2012, 0, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(1, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(short *)(a + 0xa); q2 = *(short *)(a + 0x12); q0 = 1;
          __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_SetAnim(2, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(short *)(a + 0xa); q2 = *(short *)(a + 0x12); q0 = 2;
          __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_SetAnim(3, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(short *)(a + 0xa); q2 = *(short *)(a + 0x12); q0 = 3;
          __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneWait(0x14);
    { PIN1; q0 = 0x12; __Func_8092adc(q0, 0xa0 << 7, 0); }  /*69*/
    { PIN3; q2 = (int)gOvl_0200a5c0; q0 = 0;
      __Func_8092a1c(q0, 0x10013, (void *)q2); }  /*70*/
    { PIN2; q0 = 0x13; q1 = 0xd5 << 2; __Func_80921c4(q0, q1, 0x286); }  /*71*/
    { PIN2; q0 = 0x13; q1 = 0xd5 << 2; __Func_80921c4(q0, q1, 0x29a); }  /*72*/
    { PIN2; q0 = 0x13; q1 = 0xd8 << 2; __Func_80921c4(q0, q1, 0xa8 << 2); }  /*73*/
    { PIN1; q0 = 0x13; __Func_8092adc(q0, 0x80 << 5, 0xa); }  /*74*/
    __Func_80925cc(0x13, 1);
    __CutsceneWait(0xa);
    __Func_8093040(0x13, 0, 0xa);
    { PIN2; q0 = 0x13; q1 = 0x376; __Func_80921c4(q0, q1, 0xb1 << 2); }  /*75*/
    { PIN3; q0 = 0x13; q1 = 0x37e; q2 = 0xbf << 2; __Func_809218c(q0, q1, q2); }  /*76*/
    { PIN2; q0 = 0; q1 = 0x37e; __Func_80921c4(q0, q1, 0xbf << 2); }  /*77*/
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x322);
    if (({ PIN1; q0 = 0x84f; __GetFlag(q0); }) == 0) {
        __SetFlag(0x84f);
        __SetFlag(0x84a);
    }
    __Func_8091e9c(6);
    __CutsceneEnd();
}
