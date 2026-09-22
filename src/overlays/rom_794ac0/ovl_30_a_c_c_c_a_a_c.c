/* WHOLE-FILE CONVERSION of asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_a_c.s --
 * both of its functions, no split and no linker change.
 *
 *   OvlFunc_899_2008de4  529 insns  1380 bytes  534 encodings / 151 relocations
 *   OvlFunc_899_2009348  608 insns  1608 bytes  622 encodings / 174 relocations
 *
 * Merged as one TU: 1137 = 1137 instructions, 0 differing, and 1380 + 1608 =
 * 2988 bytes is exactly the merged TU's size.
 *
 * OvlFunc_899_2009348 needs _AREA_15, WHICH WAS ALREADY IN area.sym (added in
 * batch 279 for OvlFunc_899_200adb4, the same overlay): `ldr r5, =0x15` feeding
 * __Func_8091f90(k, 0x11) / __Func_8091fa8(k, 0x10).  Against the plain reference
 * this object reports exactly 1 differing encoding plus one extra relocation --
 * that literal -- and against a symbolised copy of the reference it is OK.  No new
 * symbol tell was found.
 *
 * ================================================================
 * A COMMUTATIVE OP'S DESTINATION IS SOURCE-REACHABLE, AND THE CURE NEEDS BOTH
 * HALVES: THE CONSTANT HARD-PINNED *AND* WRITTEN AS THE ASSIGNMENT TARGET
 * ================================================================
 *
 * OvlFunc_899_2008de4 plateaued at 2 of 534 across NINE spellings and EIGHT
 * flags: ROM `orr r6, r3 / strb r6` against ours `orr r3, r6 / strb r3`, where r6
 * holds a CSE'd 1.  arm.md's *thumb_andsi3_insn and *thumb_iorsi3 are
 * structurally identical (`%0` tie, `%` commutative), so the operand order is
 * FIXED BEFORE RELOAD and no allocation lever can reach it.  -fno-regmove
 * REGRESSING to 8 confirms regmove was already firing correctly at three of the
 * four sites.
 *
 *     register int one __asm__("r6");
 *     one = 1;
 *     __MapActor_GetActor(0)[0x5a] |= one;   // orr r3, r6 -- byte is dest
 *     p = __MapActor_GetActor(1);
 *     one |= p[0x5a];  p[0x5a] = one;        // orr r6, r3 -- constant is dest
 *
 * Measured: unpinned 7, pinned-but-not-destination 2, both together 0.  Neither
 * half alone reaches it.
 *
 * THE do { } while (0) BLOCK BOUNDARY IS PER-SITE AND ITS SIGN FLIPS.
 * OvlFunc_899_2009348 had two pool loads hoisted across calls into the first
 * stall slot (.23.sched2 shows r5=0x12dc taking clocks 33-34 of a 31-cycle call
 * shadow).  Against a baseline of 4: barrier on cursor 1 only -> 2; on cursor 2
 * only -> 6; on BOTH -> 4, and the both-barriers version fixes BOTH loads while
 * disturbing exactly one argument fill, which a pin then closes.  A BARRIER THAT
 * LOOKS HARMFUL ALONE CAN BE REQUIRED IN COMBINATION -- test jointly, not singly.
 *
 * A DESCENDING PIN3 FILL WAS EXACT WHERE ASCENDING FAILED, on a complete q0-q2
 * set: `q2 = 0; q1 = 0xa; q0 = 2;` -> 0, ascending -> 4, the two mixed orders -> 2.
 * Qualifies batch 280's "ascending when the pin set is q0-first-and-complete"
 * once more -- completeness is not sufficient.
 *
 * `m` in OvlFunc_899_2008de4 needs `register int m __asm__("r8")`, AND HERE gcc
 * IS CORRECT WHILE THE ROM IS NOT.  .17.lreg prices pseudo 32 at LO_REGS:4
 * against HI_REGS:30 and .18.greg puts it in r6; r8 costs
 * `mov r3,#0x16 / add r8,r3 / mov r0,r8` where r6 costs `add r6,#0x16`.  Hoisting
 * the assignment -- the obvious non-pin route -- left it at 27; the pin took it
 * to 2.
 *
 * Pin counts at a greedy-drop fixpoint with drops verified JOINTLY and
 * cumulatively: 19 of 25 kept in the first function, 30 of 39 in the second.
 *
 * Flag sweep on the first function's residue, all inert or worse: -fno-regmove 8,
 * -fno-expensive-optimizations 14, -fno-force-mem 11, five others inert at 2.
 * No per-file Makefile override applies to this stem (rom_794ac0 has exactly one
 * explicit rule and it is ovl_30_a_c_a_c_a.o).
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_801776c(int id, int b);
extern void __Func_80917d0(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8097adc(void);
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_15;
extern unsigned char gScript_899__0200d248[];
extern unsigned char gScript_899__0200d2ac[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091fa8(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);
extern void OvlFunc_899_200c658(int a, int b);
extern void OvlFunc_899_200c684(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_899_2008de4(void)
{
    register int m __asm__("r8");
    register int one __asm__("r6");
    unsigned char *p;

    __CutsceneWait(0x14);
    __MapActor_GetActor(2)[0x5b] = 0;
    __MapActor_Jump(2, 4, 0);
    __CutsceneWait(0x28);
    m = 0x125f;
    __MessageID(m);
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x32);
    OvlFunc_899_200c5f4(2, 0x1e);
    { PIN3; q0 = 2; q1 = 0xbc << 1; q2 = 0xc4 << 1; __Func_80921c4(q0, q1, q2); }
    OvlFunc_899_200c658(2, 0);
    __CutsceneWait(0x28);
    OvlFunc_899_200c684();
    __Func_80925cc(0, 1);
    __MapActor_GetActor(0)[0x5a] &= 0xfe;
    { PIN3; q0 = 0; q1 = 0xc0 << 1; q2 = 0xd4 << 1; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    one = 1;
    __MapActor_GetActor(0)[0x5a] |= one;
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 1; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x1e);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_80925cc(2, 1);
    __CutsceneWait(0xa);
    OvlFunc_899_200c63c(2, 3, 0x14);
    __ActorMessage(2, 0);
    __Func_80925cc(1, 2);
    __CutsceneWait(0xa);
    OvlFunc_899_200c60c(1, 0, 0x1e);
    __Func_80925cc(0, 2);
    __CutsceneWait(0xa);
    OvlFunc_899_200c60c(0, 1, 0x28);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_8092c40(2, 0);
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    __Func_8091c7c(0, 0);
    __CutsceneWait(0x1e);
    OvlFunc_899_200c63c(2, 3, 0xa);
    { PIN3; q0 = 2; q1 = 0xc0 << 1; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_801776c(m + 5, 1);
    __MessageID(m + 6);
    __Func_8092848(2, 1, 0);
    OvlFunc_899_200c60c(0, 1, 0x14);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_80925cc(1, 1);
    __MapActor_GetActor(1)[0x5a] &= 0xfe;
    { PIN3; q0 = 1; q1 = 0xb0 << 1; q2 = 0xcc << 1; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    p = __MapActor_GetActor(1);
    one |= p[0x5a];
    p[0x5a] = one;
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0xb8 << 1; q2 = 0xcc << 1; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0xb8 << 1; q2 = 0xd4 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809280c(0, 1, 0);
    __MapActor_WaitMovement(2);
    OvlFunc_899_200c658(2, 1);
    OvlFunc_899_200c63c(1, 4, 0xa);
    __ActorMessage(1, 0);
    OvlFunc_899_200c684();
    OvlFunc_899_200c624(2, 0, 0x1e);
    __ActorMessage(2, 0);
    __Func_80925cc(0, 1);
    __CutsceneWait(0x1e);
    OvlFunc_899_200c63c(0, 3, 0x1e);
    OvlFunc_899_200c63c(2, 3, 0xa);
    __ActorMessage(2, 0);
    __Func_80925cc(1, 2);
    __Func_809280c(1, 0, 0);
    __ActorMessage(1, 0);
    __Func_809280c(0, 1, 0);
    OvlFunc_899_200c63c(0, 3, 0x28);
    __Func_809280c(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x1e);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(2, 3, 0x14);
    __ActorMessage(2, 0);
    __MapActor_Emote(1, 0x103, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c60c(1, 0, 0xa);
    __ActorMessage(1, 0);
    __MapActor_Emote(2, 0x80 << 1, 0);
    __CutsceneWait(0x3c);
    __ActorMessage(2, 0);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_801776c(m + 0xd, 1);
    __MessageID(m + 0xe);
    __Func_8092848(2, 0, 0);
    __Func_809280c(1, 0, 0);
    __MapActor_Emote(0, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c658(2, 0);
    __Func_8097adc();
    OvlFunc_899_200c5f4(1, 0x1e);
    OvlFunc_899_200c684();
    __Func_8092848(2, 1, 0);
    OvlFunc_899_200c63c(2, 3, 0xa);
    OvlFunc_899_200c5f4(2, 0x14);
    __Func_8092848(2, 0, 0);
    OvlFunc_899_200c63c(0, 3, 0x14);
    __Func_80925cc(1, 1);
    __CutsceneWait(0x14);
    OvlFunc_899_200c63c(1, 4, 0xa);
    OvlFunc_899_200c5f4(1, 0x1e);
    __Func_809280c(2, 1, 0);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x1e);
    OvlFunc_899_200c624(0, 1, 0xa);
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x1e);
    __Func_809280c(0, 2, 0);
    __Func_809280c(1, 2, 0);
    __Func_809280c(2, 0, 0);
    __MapActor_Emote(2, 0x83 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(2, 0xa);
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x14);
    OvlFunc_899_200c63c(2, 4, 0x14);
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(1, 0x101, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(2, 3, 0xa);
    OvlFunc_899_200c5f4(2, 0x1e);
    __MapActor_Emote(0, 0x105, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(2, 4, 0xa);
    OvlFunc_899_200c5f4(2, 0x1e);
    OvlFunc_899_200c63c(2, 3, 0xa);
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x14);
    __Func_80917d0(2, 1);
    m += 0x16;
    __MessageID(m);
    __Func_80925cc(2, 1);
    __ActorMessage(2, 0);
}

void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_899_2009348(void)
{
    int m;
    int m2;
    int k;

    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80925cc(0, 3);
    __CutsceneWait(0x14);
    do { } while (0);
    m = 0x12c5;
    __Func_801776c(m, 1);
    m += 1;
    __MessageID(m);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(8, 1);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(8, 3, 0x28);
    { PIN3; q0 = 0; q1 = 0xc6 << 2; q2 = 0xdc << 1; __Func_80921c4(q0, q1, q2); }
    OvlFunc_899_200c60c(0, 8, 0x14);
    { PIN3; q0 = 1; q1 = 0xc6 << 18; q2 = 0xdc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc6 << 18; q2 = 0xdc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xca << 2; q2 = 0xd8 << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc6 << 2; q2 = 0xe4 << 1; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(1);
    __Func_809280c(1, 8, 0);
    __MapActor_WaitMovement(2);
    OvlFunc_899_200c60c(2, 8, 0x3c);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(8, 0x14);
    __MapActor_SetAnim(2, 3);
    OvlFunc_899_200c63c(1, 3, 0x1e);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(8, 0x14);
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    OvlFunc_899_200c5f4(8, 0x1e);
    { PIN3; q0 = 8; q1 = 0xd0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_8092adc(8, 0xb0 << 8, 0);
    __CutsceneWait(0x28);
    __Func_8092adc(8, 0xd0 << 8, 0);
    __CutsceneWait(0x28);
    OvlFunc_899_200c60c(8, 0, 0x14);
    OvlFunc_899_200c63c(8, 4, 0x1e);
    OvlFunc_899_200c5f4(8, 0x14);
    __Func_80925cc(2, 1);
    OvlFunc_899_200c5f4(2, 0x28);
    { PIN3; q0 = 0xa; q1 = 0xba << 18; q2 = 0xcc << 17; __MapActor_SetPos(q0, q1, q2); }
    __PlaySound(0x3d);
    OvlFunc_899_200c5f4(0xa, 0x14);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    __Func_809259c(2, 1);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x1e);
    __Func_809280c(0, 0xa, 0);
    __Func_809280c(1, 0xa, 0);
    __Func_809280c(2, 0xa, 0);
    OvlFunc_899_200c60c(8, 0xa, 0x28);
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 9; q2 = 0xc0 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 9; q2 = 0xc0 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xba << 18; q2 = 0xcc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xba << 18; q2 = 0xcc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc6 << 2; q2 = 0xd0 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0xa, 0xc0 << 6, 0);
    __CutsceneWait(0x46);
    __Func_809280c(0, 0xa, 0);
    __Func_809280c(1, 0xa, 0);
    __Func_809280c(2, 0xa, 0);
    __Func_809280c(8, 0xa, 0);
    __MapActor_SetBehavior(0xb, gScript_899__0200d248);
    __CutsceneWait(0x28);
    __MapActor_SetBehavior(0xc, gScript_899__0200d2ac);
    __MapActor_WaitScript(0xc);
    { PIN3; q0 = 0xb; q1 = 0x80 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xc, 0x80 << 6, 0);
    __CutsceneWait(0x28);
    OvlFunc_899_200c63c(0xa, 4, 0x14);
    OvlFunc_899_200c5f4(0xa, 0x14);
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0xa);
    OvlFunc_899_200c5f4(0xb, 0x14);
    __Func_80925cc(0xc, 1);
    __CutsceneWait(0xa);
    OvlFunc_899_200c5f4(0xc, 0x28);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(0xa, 4, 0x14);
    OvlFunc_899_200c5f4(0xa, 0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(1, 0x101, 0);
    __CutsceneWait(0x3c);
    __MapActor_Emote(8, 0x80 << 1, 0);
    __CutsceneWait(0x28);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(8, 0x14);
    __Func_809280c(0, 8, 0);
    __Func_809280c(1, 8, 0);
    OvlFunc_899_200c60c(2, 8, 0x14);
    OvlFunc_899_200c63c(8, 4, 0x1e);
    OvlFunc_899_200c5f4(8, 0x14);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __Func_809280c(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x1e);
    OvlFunc_899_200c5f4(1, 0x14);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(8, 0x14);
    __Func_80925cc(0xa, 1);
    __CutsceneWait(0xa);
    OvlFunc_899_200c5f4(0xa, 0x14);
    __Func_809280c(0, 0xa, 0);
    __Func_809280c(1, 0xa, 0);
    OvlFunc_899_200c60c(2, 0xa, 0x14);
    OvlFunc_899_200c63c(0xb, 3, 0x14);
    OvlFunc_899_200c5f4(0xb, 0x14);
    OvlFunc_899_200c63c(0xc, 4, 0x14);
    OvlFunc_899_200c5f4(0xc, 0x14);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(2, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(0xa, 3, 0x14);
    __Func_8092c40(0xa, 0);
    __CutsceneWait(0x32);
    __Func_809280c(2, 0, 0);
    OvlFunc_899_200c60c(1, 0, 0x1e);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x28);
        __Func_809280c(1, 0xa, 0);
        __Func_809280c(2, 0xa, 0);
        __Func_80925cc(0xa, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0xa, 0);
    } else {
        __CutsceneWait(0x28);
        __Func_809280c(1, 0xa, 0);
        { PIN3; q2 = 0; q1 = 0xa; q0 = 2; __Func_809280c(q0, q1, q2); }
        do { } while (0);
        m2 = 0x12dc;
        __MessageID(m2);
        __Func_80925cc(0xa, 2);
        __CutsceneWait(0x14);
        m2 -= 3;
        __ActorMessage(0xa, 0);
        __MessageID(m2);
    }
    __MapActor_Emote(0xb, 0x103, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(0xb, 0x14);
    OvlFunc_899_200c63c(0xc, 4, 0x14);
    OvlFunc_899_200c5f4(0xc, 0x1e);
    OvlFunc_899_200c624(0xa, 0xb, 0x1e);
    __MapActor_SetAnim(0xa, 3);
    OvlFunc_899_200c63c(0xb, 3, 0x1e);
    OvlFunc_899_200c624(0xa, 0xc, 0x1e);
    __MapActor_SetAnim(0xa, 3);
    OvlFunc_899_200c63c(0xc, 3, 0x28);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xc, 0xc0 << 6, 0);
    __CutsceneWait(0x14);
    OvlFunc_899_200c63c(0xa, 4, 0x14);
    __ActorMessage(0xa, 0);
    __SetFlag(0x854);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
    k = (int)(&_AREA_15);
    __Func_8091f90(k, 0x11);
    __Func_8091fa8(k, 0x10);
    do { } while (0);
    gState[0x22b] = 3;
    __Func_8091eb0(0xc, 5);
    __CutsceneEnd();
}
