/*
 * OvlFunc_936_200a6c0 -- a 1066-instruction cutscene: 249 calls, three
 * `if (actor != 0)` guards, one two-armed `if` on __Func_8091c7c, one pool
 * jump and four writes through iwram_3001ebc.  Message base 0x256f.
 *
 * The ROM holds FOUR values in callee-saved registers (`hi=9 hiv=2`):
 * r5 the actor pointer already advanced by +0x66, then gScript_936__0200c230;
 * r6 gScript_936__0200c21c, then 0x80 << 7; r8 &iwram_3001ebc; r10 0xe0 << 1.
 * Plain C evicts SEVEN.  The ladder, as d.py's aligned instruction diff:
 *
 *     plain C transcription ................................. 403
 *     + all 223 pinnable argument sites pinned ...............  76
 *     + `int w = 0x80 << 7` ..................................  58
 *     + the two 6-argument calls' STACK words pinned .........  46
 *     + `p = __MapActor_GetActor(n) + 0x66` folded ...........  28
 *     + a SECOND actor local `p`, and the three SetPos sites
 *       pinned in the ROM's fill order (worth 22 TOGETHER;
 *       0 and 6 apart) .......................................   2
 *     + __Func_8092c40's arguments filled r1 before r0 .......   0
 *
 * Four levers earn their comment:
 *
 * 1. `w`.  `*(unsigned short *)(p + 6) = 0x80 << 7;` and
 *    `__Func_80933d4(0x80 << 10, 0x80 << 7);` are NOT a common subexpression:
 *    the store's operand is HImode and the argument's is SImode, so gcc emits
 *    a halfword pool load for one and a fresh mov/lsl for the other, and the
 *    function comes out one callee-saved register short -- `pop {r3}` where
 *    the ROM has `pop {r3, r5}`.  An `int` local makes it one value in r6.
 *    This is the SHARING direction of the recorded HImode rule, whose known
 *    cases all want the constant REMATERIALISED at the store instead; the
 *    push mask is what tells the two apart.
 *
 * 2. The stack words.  The recorded lever for a call's stack arguments is to
 *    name them as locals in the ROM's store order.  Under a full pin set that
 *    is the wrong spelling: with r0-r3 all pinned gcc has no caller-saved
 *    register left and routes the local through r4, one word at a time,
 *    interleaved with the r0-r3 fill.  Passing them as PINNED register
 *    variables by value forces both `str [sp]` stores ahead of the register
 *    fill, which is the ROM's order.
 *
 * 3. `+ 0x66` folded into the assignment.  The ROM destroys the actor pointer
 *    with `add r5, #0x66`; with the offset inside the store gcc keeps two
 *    pseudos and copies.  The recorded cure -- inline the call into the store
 *    -- is unavailable here because __Random() sits between the two, so the
 *    offset goes into the ASSIGNMENT instead.  Third spelling of "write the
 *    destruction, not the expression".
 *
 * 4. Two actor locals.  One `unsigned char *` for every actor pointer gives
 *    the whole pseudo a call-SAVED register, because one of its segments
 *    crosses __Random; the three guards then pay `mov r5, r0 / cmp r5, #0`
 *    where the ROM tests r0.  `a` crosses the call and wants r5; `p` never
 *    does and wants r0.
 *
 * -fno-schedule-insns2 REGRESSES (403 -> 561 plain, 0 -> 363 here): sched2 is
 * already producing the ROM's order and alias is the wrong axis.  Eighth
 * function to say so and the first outside rom_79c738.
 *
 * 112 of 218 pins survive a per-site drop-to-fixpoint (two rounds).
 * objcmp: 2756 bytes, 1098 encodings and 253 relocations identical.
 */
/* ovl_30_c_c_c_a_c_c_c_c_c_c.c  --  OvlFunc_936_200a6c0 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_936__0200c21c[];
extern unsigned char gScript_936__0200c230[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_936_200a6c0(void)
{
    unsigned char *a;
int w;
unsigned char *p;
    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    { PIN3; q0 = 9; q1 = 0xdc << 17; q2 = 0x20a0000; __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(9);
    __Actor_SetSpriteFlags(a, 0);
    { PIN4; q0 = 0xdc << 17; q1 = -1; q2 = 0x20a0000; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapTransitionIn();
    { PIN2; q0 = 0x19999; q1 = 0x3333; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xdc << 17; q1 = -1; q2 = 0xc8 << 17; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    __PlaySound(0x8d);
    { PIN3; q0 = 9; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xdc << 1; q2 = 0xc8 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN2; q0 = 0xc0 << 8; q1 = 0xc0 << 5; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xdc << 17; q1 = -1; q2 = 0x96 << 17; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xdc << 1; q2 = 0x96 << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(9, 0);
    __PlaySound(0x121);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xb; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1b70000; q2 = 0x99 << 17; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_Jump(0xb, 4, 0);
    { PIN3; q0 = 0xb; q1 = 0x1b7; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xd0 << 1; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc8 << 1; q2 = 0x80 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __Func_80933f8(0xcd << 17, -1, 0x8c << 17, 1);
    { PIN3; q0 = 0xa; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1b70000; q2 = 0x99 << 17; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_Jump(0xa, 4, 0);
    { PIN3; q0 = 0xa; q1 = 0x1b7; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 1; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc2 << 1; q2 = 0x87 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x1b70000; q2 = 0x99 << 17; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_Jump(0, 4, 0);
    { PIN3; q0 = 0; q1 = 0x1b7; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xd0 << 1; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc2 << 1; q2 = 0x96 << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    a = __MapActor_GetActor(0) + 0x66;
    *(unsigned short *)a = __Random() * 5 >> 12;
    __MapActor_SetBehavior(0, gScript_936__0200c21c);
    __MapActor_Jump(0xb, 2, 0x14);
    __MapActor_DoAnim(0xb, 3);
    __MessageID(0x256f);
    __Func_8093040(0xb, 0, 0xa);
    __MapActor_DoAnim(0xa, 3);
    p = __MapActor_GetActor(0);
    if (p != 0)
        { PIN3; q1 = *(int *)(p + 8); q2 = *(int *)(p + 0x10); q0 = 1; __MapActor_SetPos(q0, q1, q2); }
    p = __MapActor_GetActor(0);
    if (p != 0)
        { PIN3; q1 = *(int *)(p + 8); q2 = *(int *)(p + 0x10); q0 = 2; __MapActor_SetPos(q0, q1, q2); }
    p = __MapActor_GetActor(0);
    if (p != 0)
        { PIN3; q1 = *(int *)(p + 8); q2 = *(int *)(p + 0x10); q0 = 3; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xbd << 1; q2 = 0x9b << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc8 << 1; q2 = 0x90 << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xcd << 1; q2 = 0x9a << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    a = __MapActor_GetActor(1) + 0x66;
    *(unsigned short *)a = __Random() * 5 >> 12;
    a = __MapActor_GetActor(2) + 0x66;
    *(unsigned short *)a = __Random() * 5 >> 12;
    a = __MapActor_GetActor(3) + 0x66;
    *(unsigned short *)a = __Random() * 5 >> 12;
    __MapActor_SetBehavior(1, gScript_936__0200c21c);
    __MapActor_SetBehavior(2, gScript_936__0200c21c);
    __MapActor_SetBehavior(3, gScript_936__0200c21c);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2002; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xa, 0, 0xa);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(2);
    __MapActor_SetIdle(3);
    __WaitFrames(1);
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    __Func_8093040(0xa, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    __Func_8093040(0xa, 0, 0xa);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(1, 0, 0xa);
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xb, 2);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    __Func_8093040(0xa, 0, 0xa);
    __MapActor_DoAnim(0xb, 3);
    __MapActor_Surprise(2, 0x81 << 1);
    __CutsceneWait(0x28);
    __Func_809259c(2, 2);
    __Func_8093040(0x2002, 0, 0xa);
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 4);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x84 << 1; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0xa, 0, 0xa);
    __Func_80925cc(1, 2);
    __Func_8093040(1, 0, 0xa);
    { PIN3; q0 = 0xb; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 4);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xb, 4);
    __Func_8093040(0xb, 0, 0xa);
    __MapActor_DoAnim(0xa, 4);
    __Func_8093040(0xa, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 3);
    { PIN2; q1 = 0; q0 = 0xa; __Func_8092c40(q0, q1); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 3;
    } else {
        __CutsceneWait(0x14);
        __Func_80925cc(0xb, 2);
        __Func_8093040(0xb, 0, 0x28);
        __Func_8093040(0xb, 0, 0xa);
        { PIN3; q0 = 3; q1 = 0x83 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0x2003, 0, 0xa);
    }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    { PIN2; q0 = 0x80 << 9; q1 = 0x80 << 6; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x8c << 17; q1 = -1; q2 = 0xc8 << 16; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0xa; q1 = 0x14d; q2 = 0xde; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x8e << 1; q2 = 0xc6; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    __Func_8092adc(8, 0, 0xa);
    __MapActor_DoAnim(8, 3);
    { PIN3; q0 = 8; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x86 << 1; q2 = 0xc6; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(8, 2);
    __PlaySound(0x7d);
    { PIN4; q3 = 2; q2 = 1; __CopyMapTiles(0x47, 0x3c, 0x4c, 0xb, q3, q2); }
    { PIN4; q3 = 0x10; q2 = 0xb; __Func_8010704(0x47, 0x3c, 2, 1, q3, q2); }
    __CutsceneWait(0x14);
    __Func_80921c4(8, 0xf6, 0xc6);
    __Func_8092adc(8, 0, 0x14);
    *(__Func_8093554() + 0x55) = 0;
    __Func_80933d4(0x9999, 0x1333);
    { PIN4; q0 = 0xf8 << 16; q1 = -1; q2 = 0xaa << 16; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0xa; q1 = 0x87 << 1; q2 = 0xc6; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x87 << 1; q2 = 0xae; __Func_80921c4(q0, q1, q2); }
    __Func_80921c4(0xa, 0xe0, 0xaa);
    __Func_80921c4(0xa, 0xd2, 0x9e);
    __Func_80921c4(0xa, 0xf6, 0x94);
    __Func_80921c4(0xa, 0xf6, 0x8e);
    __MapActor_SetPos(0xa, 0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN3; q0 = 9; q1 = 0xdc << 17; q2 = 0xaa << 17; __MapActor_SetPos(q0, q1, q2); }
    p = __MapActor_GetActor(9);
    w = 0x80 << 7;
    *(unsigned short *)(p + 6) = w;
    { PIN4; q0 = 0xbe << 17; q1 = -1; q2 = 0x8c << 17; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0xa);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    { PIN3; q0 = 0xb; q1 = 0xd2 << 1; q2 = 0x8d << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xd2 << 1; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1b7; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1b7; q2 = 0x99 << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetBehavior(1, gScript_936__0200c230);
    __MapActor_SetBehavior(2, gScript_936__0200c230);
    __MapActor_RunScript(3, gScript_936__0200c230);
    { PIN4; q0 = 0xcd << 17; q1 = -1; q2 = 0x96 << 17; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0; q1 = 0xd0 << 1; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x1b7; q2 = 0x9c << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x1b7; q2 = 0x99 << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0, 0, 0);
    __PlaySound(0x8d);
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    { PIN4; q0 = 0xdc << 17; q1 = -1; q2 = 0xd2 << 17; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 9; q1 = 0xdc << 1; q2 = 0xd2 << 1; __Func_80921c4(q0, q1, q2); }
    __Func_80933d4(0x80 << 10, w);
    __Func_80933f8(0xdc << 17, -1, 0x96 << 18, 1);
    { PIN3; q0 = 9; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xdc << 1; q2 = 0xfa << 1; __Func_80921c4(q0, q1, q2); }
    __Func_809218c(9, 0xdc << 1, 0x96 << 2);
    __PlaySound(0x121);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x18;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xa);
    __CutsceneEnd();
}
