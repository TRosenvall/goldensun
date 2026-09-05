// fakematch
/* OvlFunc_952_2008674  --  0x02008674
 *   [asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c.s, 1st of 6]
 *
 * 433 instructions of straight-line cutscene script with one two-armed
 * conditional and three guarded actor fetches at the tail.
 * Byte-exact: 1156 bytes, 443 encodings and 125 relocations identical
 * (tools/objcmp.py; tryc's OK is weak here -- the ref holds six functions,
 * so the size check is skipped and the pool sits inside the function).
 *
 * CONSTANT-CSE, AND THE PROLOGUE PICKS THE CURE -- BY CONTENT, NOT WIDTH.
 * `push {r5, r6, lr}` looks like a two-named-local function.  It is not:
 * r5 is a scratch slot recycled five times (the byte 0, then 0x4000000, then
 * the byte 1, then 0x1c8, then 0xfffd0000) and r6 holds &iwram_3001ebc.  NO
 * script constant is kept anywhere, so every repeated one is rebuilt at every
 * use and only PINS reach it.  Plain C is 445 lines against 439 with 424
 * differing, a widened `push {r5, r6, r7, lr}` and `mov r7, r8` traffic:
 * cse_main commons 0xf0<<15, 0x80<<9/0x80<<8 (2 sites), 0x81<<1 (2),
 * 0xe0<<8 (2), 0xc0<<8 (2), 0x80<<7 (2) and 0x13333/0x9999 (3) into pseudos
 * that must be callee-saved because every use straddles a `bl`.
 *
 * SEVENTEEN PINS, at a greedy fixpoint over 25 candidate sites.  All 25
 * expensive-constant call sites were pinned first (that also matches).
 * Stripping one at a time leaves EIGHT individually inert -- __Func_8092adc's
 * 0x80<<6 site, its FIRST 0x80<<7 site, both 0xc0<<8 sites, the SECOND
 * 0xe0<<8 site, __Func_8091c7c(-1, 0), __MapActor_Emote(0, 0x105, 0x3c) and
 * __Func_8092304(0, -0x20, 0).  All eight are jointly removable, and a second
 * one-at-a-time round over the surviving 17 finds every one load-bearing.
 * Verified as a set under objcmp, in this file's final shape -- not by line
 * count and not in the scaffolded shape it was first measured in.
 *
 * THE FILLS ARE UNIFORM, NOT TRANSCRIBED.  Every pinned site is written one
 * statement per argument, ascending q0..q3, whole value per statement, even
 * where the ROM emits the site differently -- `mov r1 / mov r2 / mov r0 /
 * lsl r1 / lsl r2` at __MapActor_SetPos, `mov r1 / lsl r1 / mov r2 / mov r0`
 * at one __Func_8092adc and `mov r1 / mov r2 / lsl r1 / mov r0` at another,
 * `mov r0 / mov r1 / strh / neg r0` at __ActorMessage.  sched2 reproduces all
 * of them from the one spelling.
 *
 * ONE SITE IS A REAL EXCEPTION: __Func_8092c40(1, 0).  The ROM emits
 * `mov r1, #0 / mov r0, #1`; a uniform ascending pin gives the transposed
 * pair, and with the other 24 sites pinned that was the ONLY residue
 * (439/439 lines, 2 differing).  Writing the fill descending -- `q1 = 0;
 * q0 = 1;` -- is exact.  Same site signature as the documented exception in
 * src/overlays/rom_7f148c/ovl_30_c_c_c_a_a_b.c.
 *
 * THE MESSAGE BASE IS A SYMBOL, AND THE RELOCATIONS SAY SO.  objcmp shows the
 * reference carries `R_ARM_ABS32 _MSG_2280` at offset 0x70; `__MessageID(0x2280)`
 * drops it and is 440 lines with 438 differing.  _MSG_2280 is already in
 * message.sym.  0x105, 0x1e666/0xf333, 0x13333/0x9999, 0x1140 and 0x140 carry
 * no relocation on either side and are bare literals.
 *
 * NO SCAFFOLDING LOCALS.  Four were carried through the pin sweep -- a
 * `unsigned short *` for 0x4000000, an `int` for the 0x1c8 iwram offset, an
 * `int` for 0xfffd0000 and an `int` for the two 0/1 byte stores.  Each is
 * byte-identical when removed, alone and together, so none of them ships: gcc
 * commons all four itself and gives them the ROM's r5.  `a` survives only for
 * the three guarded fetches, whose value is genuinely live across the
 * `cmp`/`beq`; naming the two `+= 0xfffd0000` fetches as well is byte-identical
 * and is left out, and reusing ONE fetch for both is 437 lines / 314 differing.
 *
 * No Makefile rule or wildcard names rom_7d768c, so the tree default -O2
 * applies; objcmp against the original asm/ path and a scratch copy of the ref
 * agree, which is the wildcard check.  tryc freshness confirmed with
 * -fno-omit-frame-pointer as a positive control (echoed, 453 lines /
 * 444 differing -- the probe moves).
 *
 * LANDING NEEDS A SPLIT.  The .s holds six functions and this is the FIRST;
 * overlays/rom_7d768c/overlay.ld:29 names the single .o.  No .section .data
 * in the file, so tools/split_s.py's ordinary path applies.
 */
extern int _MSG_2280;
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_Jump(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);


#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_952_2008674(void)
{
    unsigned char *a;

    __CutsceneStart();
    __MessageID((int)(&_MSG_2280));
    __MapActor_GetActor(0)[0x54] = 0;
    __MapActor_GetActor(0xa)[0x54] = 0;
    __WaitFrames(1);
    *(unsigned short *)(0x80 << 19) = 0x1140;
    { PIN2; q0 = -1; q1 = 0;
      __ActorMessage(q0, q1); }
    *(unsigned short *)(0x80 << 19) = 0x140;
    __MapActor_GetActor(0)[0x54] = 1;
    __MapActor_GetActor(0xa)[0x54] = 1;
    __MapActor_SetAnim(0, 0x1f);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    { PIN3; q0 = 1; q1 = 0xf0 << 15; q2 = 0xd0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xd0 << 15; q2 = 0xa0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xf0 << 15; q2 = 0xf0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8092adc(1, 0, 0);
    __Func_8092adc(3, 0, 0);
    { PIN3; q0 = 2; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x3c;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x18;
    { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(3, 0x10, 0);
    __Func_8092adc(3, 0x80 << 6, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    *(int *)(__MapActor_GetActor(0) + 0x10) += 0xfffd0000;
    *(int *)(__MapActor_GetActor(0) + 0x40) += 0xfffd0000;
    __MapActor_SetAnim(0, 0x20);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0, 0x22);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 0x21);
    __CutsceneWait(0x32);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(0, 0x105, 0x3c);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x83 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(1, 0x80 << 7, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x2d);
    __Func_8092adc(1, 0, 0);
    __Func_8092adc(2, 0xe0 << 8, 0);
    __CutsceneWait(0x1e);
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(-1, 0) != 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x22);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(1, 3);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x21);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(1, 3);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x21);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(1, 3);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = -0x10; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    __Func_8092adc(1, 0, 0);
    __CutsceneWait(0x23);
    __MapActor_Jump(0, 6, 0);
    { PIN3; q0 = 0; q1 = 0x1e666; q2 = 0xf333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0, -0x20, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    { PIN3; q0 = 3; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0, 3);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(2, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(1, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(3, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(3, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __MapActor_SetAnim(2, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(2, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
