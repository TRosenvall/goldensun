/*
 * OvlFunc_888_200987c -- a 1276-instruction cutscene: 394 calls, `hi=0 hiv=0`,
 * and the simplest large function in the series so far.  The only control flow
 * is one 32-iteration loop that walks actor 0xd's x and y fields down by 655 a
 * frame, and the only two callee-saved registers are that loop's counter and
 * its LICM-hoisted -655.
 *
 *     push {r5, r6, lr}   r5 = i, r6 = -655
 *
 * A faithful plain-C transcription measures 182 on d.py's aligned instruction
 * diff, EIGHT INSTRUCTIONS SHORT of the reference's 1276.  `pinall` -- every
 * one of the 394 argument sites spelled as pinned `register` variables -- is
 * worth 182 -> 0 on its own.  No second lever was needed: no local, no
 * statement-expression, no ordering knob, no control-flow rewrite.
 *
 * The loop is a plain `for (i = 0; i <= 0x1f; i++)`, `i` UNSIGNED (`bls`), and
 * `-= 655` written literally in both bodies; gcc hoists the constant into r6
 * and turns the loop into the ROM's test-at-bottom shape by itself.
 *
 * `b .L1bd0 / .pool_aligned / .L1bd0:` is BOTH a pool boundary and the loop's
 * entry edge -- the recorded warning that "a pool follows" does not settle
 * such a sequence, here confirmed: the label's other predecessor is the `bls`
 * at the bottom.  The second such sequence in the function is a pool jump only.
 *
 * -fno-schedule-insns2 REGRESSES (182 -> 446 plain, 0 -> 382 here): sched2 is
 * already producing the ROM's order.  Ninth function to say so.
 *
 * Minimisation is the cleanest in the series: the 317 sites whose every
 * argument is a bare `mov #imm8` DROP AS A SET, in one measurement, to 0.  All
 * 60 surviving pins are sites with a shifted (41) or pooled (19) argument --
 * 37 of them `__MapActor_Emote`.  That is the recorded "a site is INERT for an
 * ordering pin only when EVERY non-r0 argument is a bare `mov #imm8`" rule
 * holding exactly, where it FAILED on both of this batch's neighbours; the
 * difference is that here no pin competes with a value the ROM holds.
 *
 * objcmp: 3400 bytes, 1288 encodings and 400 relocations identical.
 */
/* ovl_30_c_c_a_a_a_c_a_c_a.c  --  OvlFunc_888_200987c */
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __ActorMessage(int slot, int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_888_200a5c4(void);
extern void OvlFunc_888_200a660(void);
extern void OvlFunc_888_200b1b8(int slot);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_888_200987c(void)
{
    unsigned char *p;
    unsigned int i;
    { PIN2; q0 = 1; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN4; q0 = 0xc0 << 16; q1 = -1; q2 = 0xa0 << 16; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __Func_809280c(1, 0, 0);
    __Func_809280c(0xc, 0, 0);
    __Func_809280c(9, 0, 0);
    __Func_809280c(0xa, 0, 0);
    __Func_8092848(0, 0xb, 0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(8, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x32);
    __Func_809280c(0, 8, 0);
    __Func_809280c(0xb, 8, 0);
    __Func_809280c(0xc, 8, 0);
    __Func_809280c(9, 8, 0);
    __Func_809280c(0xa, 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x14);
    __MessageID(0x1171);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    __Func_809280c(0xc, 1, 0);
    __Func_809259c(0xc, 2);
    __MapActor_Emote(0xc, 0x103, 0);
    __CutsceneWait(0x3c);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __Func_809280c(1, 0xc, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x28);
    __Func_809280c(1, 8, 0);
    __Func_809280c(0xc, 8, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8092adc(8, 0xd0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0x50);
    __PlaySound(0x11);
    __Func_8091200(0x10005, 1);
    __Func_8091254(0x3c);
    __CutsceneWait(0x28);
    { PIN2; q0 = 0x6666; q1 = 0xccc; __Func_80933d4(q0, q1); }
    __Func_80933f8(0xc0 << 16, -1, 0xd0 << 15, 1);
    __CutsceneWait(0x78);
    __PlaySound(0x15);
    { PIN1; q0 = 0x9a << 1; __PlaySound(q0); }
    { PIN3; q0 = 0xd; q1 = 0xc8 << 16; q2 = 0x80 << 12; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092158(0xd, 0xc8, 0x48);
    __PlaySound(0x90 << 1);
    __CutsceneWait(0x1e);
    __Func_80925cc(8, 2);
    __MapActor_SetAnim(8, 0);
    OvlFunc_888_200a5c4();
    __Func_809280c(0, 0xd, 0);
    __Func_809280c(1, 0xd, 0);
    __Func_809280c(0xb, 0xd, 0);
    __Func_809280c(0xc, 0xd, 0);
    __Func_809280c(9, 0xd, 0);
    __Func_809280c(0xa, 0xd, 0);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(0xb, 2);
    __Func_809259c(0xc, 2);
    __Func_809259c(9, 2);
    __Func_809259c(0xa, 2);
    __CutsceneWait(0x28);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0x28);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0x3c);
    __Func_8092950(0xd, 0x80 << 1);
    __PlaySound(0x11);
    __PlaySound(0x9a << 1);
    for (i = 0; i <= 0x1f; i++) {
        OvlFunc_888_200b1b8(0xd);
        __CutsceneWait(4);
        p = __MapActor_GetActor(0xd);
        *(int *)(p + 0x18) -= 655;
        p = __MapActor_GetActor(0xd);
        *(int *)(p + 0x1c) -= 655;
    }
    __PlaySound(0x90 << 1);
    __Func_8092950(0xd, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __CutsceneWait(0x28);
    __Func_80933f8(0xc0 << 16, -1, 0xa0 << 16, 1);
    __Func_8093530();
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x3c);
    __CutsceneWait(0x78);
    OvlFunc_888_200a660();
    __MapActor_SetAnim(8, 1);
    __PlaySound(2);
    __CutsceneWait(0x3c);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    __Func_809259c(0xb, 1);
    __Func_809259c(0xc, 1);
    __Func_809259c(9, 1);
    __Func_80925cc(0xa, 1);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x1e);
    __Func_809280c(8, 0, 0);
    __CutsceneWait(0x1e);
    __Func_809280c(0xc, 8, 0);
    __CutsceneWait(0x14);
    __Func_809280c(0, 8, 0);
    __Func_809280c(1, 8, 0);
    __Func_809280c(0xb, 8, 0);
    __Func_809280c(9, 8, 0);
    __Func_809280c(0xa, 8, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __Func_809280c(8, 0xc, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __Func_809280c(8, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0xb4);
    { PIN3; q0 = 0xc; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_8092848(0, 1, 0);
    __Func_8092848(9, 0xa, 0);
    __CutsceneWait(0x28);
    __Func_809280c(0, 8, 0);
    __Func_809280c(1, 8, 0);
    __Func_809280c(9, 8, 0);
    __Func_809280c(0xa, 8, 0);
    __CutsceneWait(0x28);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    __Func_809259c(0xb, 1);
    __Func_809259c(0xc, 1);
    __Func_809259c(9, 1);
    __Func_80925cc(0xa, 1);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x50);
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_80925cc(8, 1);
    __MapActor_Emote(8, 0x83 << 1, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    __Func_809259c(0xb, 1);
    __Func_809259c(0xc, 1);
    __Func_809259c(9, 1);
    __Func_80925cc(0xa, 1);
    __CutsceneWait(0x28);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x78);
    __Func_80925cc(1, 1);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_80925cc(0xc, 1);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __Func_809280c(8, 0xc, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 4);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_809280c(8, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __ActorMessage(8, 0);
    __CutsceneWait(0x28);
    __Func_80921c4(8, 0xa8, 0xb0);
    __Func_809280c(0, 8, 0);
    __Func_809280c(1, 8, 0);
    __Func_809280c(0xb, 8, 0);
    __Func_809280c(0xc, 8, 0);
    __Func_809280c(9, 8, 0);
    __Func_809280c(0xa, 8, 0);
    __Func_80921c4(8, 0xc8, 0xc8);
    __Func_809280c(0, 8, 0);
    __Func_809280c(1, 8, 0);
    __Func_809280c(0xc, 8, 0);
    __Func_8092adc(0xb, 0, 0);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xc8; q2 = 0x88 << 1; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    __MapActor_SetPos(8, 0, 0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __CutsceneWait(0x78);
    __Func_809280c(0, 9, 0);
    __Func_809280c(1, 0xa, 0);
    __Func_809280c(0xb, 9, 0);
    __Func_809280c(0xc, 9, 0);
    __CutsceneWait(0x78);
    { PIN3; q0 = 9; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_809280c(9, 0, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(9, 2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 9; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x50);
    __Func_809280c(9, 0xa, 0);
    __Func_80925cc(9, 1);
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_Emote(0xa, 0x101, 0);
    __CutsceneWait(0x3c);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_809280c(0xa, 0, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x1e);
    __Func_8092adc(0xa, 0xa0 << 7, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(9, 3);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 9; q1 = 0xc8; q2 = 0x88 << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc8; q2 = 0x88 << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_Emote(0xc, 0x105, 0);
    __CutsceneWait(0x3c);
    __Func_80921c4(0xc, 0xc8, 0x88);
    __Func_8092848(0, 0xc, 0);
    __Func_809280c(1, 0xc, 0);
    __Func_809280c(0xb, 0xc, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xc, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __Func_80921c4(0xb, 0xa8, 0xa8);
    __Func_809280c(0xb, 0xc, 0);
    __CutsceneWait(0x14);
    __Func_809280c(0, 0xb, 0);
    __Func_809280c(1, 0xb, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0xc, 4);
    __CutsceneWait(0x14);
    __Func_809280c(0, 0xc, 0);
    __Func_809280c(1, 0xc, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapActor_Emote(0xc, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(1, 0x80 << 1, 0);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __Func_809280c(0, 0xb, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0xc, 3);
}
