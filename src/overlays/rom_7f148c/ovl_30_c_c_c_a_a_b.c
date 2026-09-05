// fakematch
/* OvlFunc_966_2008218  --  0x02008218
 *   [asm/overlays/rom_7f148c/ovl_30_c_c_c_a_a.s, 1st of 3]
 *
 * 533 instructions of straight-line cutscene -- one message base, fifteen
 * turns, one guarded actor fetch at the tail and no other control flow.
 * Byte-exact: 1452 bytes, 541 encodings and 177 relocations identical.
 *
 * CONSTANT-CSE, AND THE PROLOGUE PICKS THE CURE.  The ROM's prologue is
 * `push {lr}` -- it keeps NOTHING across the body, so every repeated constant
 * is rebuilt at every use.  Plain C is 544 lines against 535 with 517
 * differing and a `push {r5, r6, lr}` + r8-r11 prologue: cse_main commons
 * 0xc0<<8 (4 sites), -0x10 (2), 0x81<<1 (4), 0x80<<7 (5), 0x80<<8 (4),
 * 0x80<<1 (3) and 0x13333/0x9999 (2), and because every use straddles a `bl`
 * the pseudos must be callee-saved.  Only PINS reach this shape; a named local
 * would measure worse, since the ROM holds no value at all.
 *
 * TWENTY-SEVEN PINS, at a greedy fixpoint over 31 candidate sites.  Stripping
 * one at a time leaves FOUR individually inert -- __MessageID(0x2694), the
 * lone 0x80<<6 __Func_8092adc, and the last 0x80<<7 and last 0xc0<<8
 * __Func_8092adc sites.  All four are jointly removable, and a second
 * one-at-a-time round over the surviving 27 finds no new inert pin.  Verified
 * as a set under objcmp, not by line count.  Note the polarity recorded
 * elsewhere: each of the four is the LAST use of its value, never the first --
 * one pin at the first use covers the later ones.
 *
 * THE FILLS ARE UNIFORM, NOT TRANSCRIBED.  Every pinned site is written one
 * statement per argument, ascending q0..q3, whole value per statement, even
 * where the ROM emits the site differently (`mov r1 / mov r2 / mov r0 / lsl r1`
 * at __MapActor_SetPos, `mov r2 / mov r3 / lsl r3 / mov r1 / neg r2 / mov r0`
 * at __Func_809233c, and so on).  sched2 reproduces all of them.
 *
 * ONE SITE IS A REAL EXCEPTION: __Func_8092c40(0x16, 0).  The ROM emits
 * `mov r1, #0 / mov r0, #0x16`; both plain C and a uniform ascending pin give
 * the transposed pair, and this was the ONLY residue once the other 30 sites
 * were pinned (535/535 lines, 2 differing).  Writing the fill descending --
 * `q1 = 0; q0 = 0x16;` -- is exact.  So the uniform rule is the default to try
 * first, not a law: measure the residue.
 *
 * 0x2694 IS A LITERAL, NOT A SYMBOL, AND THE RELOCATIONS SAY SO.  objcmp
 * reports 177 relocations identical with the id spelled as a bare int; the
 * reference carries no R_ARM_ABS32 in this function's range beyond the `bl`
 * targets.  Same for 0x101, 0x1cccc/0xe666, 0x19999/0xcccc and 0x13333/0x9999.
 *
 * No Makefile rule names rom_7f148c and no wildcard reaches it, so the tree
 * default -O2 applies; objcmp against the original asm/ path and a scratch
 * path agree, which is the wildcard check.  tryc freshness confirmed with
 * -fno-omit-frame-pointer as a positive control (echoed, 538 lines /
 * 538 differing -- the probe moves).
 *
 * LANDING NEEDS A SPLIT.  The .s holds three functions and this is the FIRST;
 * overlays/rom_7f148c/overlay.ld:27 names the single .o.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);


#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_966_2008218(void)
{
    unsigned char *p;

    __CutsceneStart();
    __MessageID(0x2694);
    { PIN3; q0 = 0; q1 = 0xf8 << 16; q2 = 0xd4 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(8, 0);
    __MapActor_SetAnim(9, 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN4; q0 = 0x16; q1 = 8; q2 = -0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(0x16);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x16; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0; q2 = -0x10;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_809280c(0x16, 8, 0x28);
    __Func_809280c(0x16, 9, 0x28);
    __Func_809280c(0x16, 8, 0x28);
    __CutsceneWait(0xa);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 0x16;
      __Func_8092c40(q0, q1); }
    __Func_8091c7c(0, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0x16, 8, 0x1e);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __MapActor_SetAnim(8, 1);
    __CutsceneWait(0x14);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0x16, 9, 0);
    __Func_809280c(0, 9, 0x1e);
    __MapActor_SetAnim(9, 1);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x84 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(9, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(0x16, 9, 0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x16; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x1cccc; q2 = 0xe666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 1; q2 = 0xb4 << 1;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092adc(0x16, 0, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x16; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x16; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0x16, 0x80 << 6, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(9, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(9, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x16; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 1; q2 = 0xc0 << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x16, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x16, 0, 0x10);
    __CutsceneWait(0xa);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x101; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(0x16, 8, 0x28);
    __Func_809280c(0x16, 9, 0x28);
    { PIN3; q0 = 0x16; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __ActorMessage(9, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0x16, 0, 0);
    __Func_809280c(0, 9, 0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809280c(0, 8, 0x46);
    __Func_8092adc(0x16, 0x80 << 7, 0);
    __CutsceneWait(0x28);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __CutsceneWait(0x28);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x16; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0x16, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x16, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x16);
    __MapActor_SetPos(0x16, 0, 0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
