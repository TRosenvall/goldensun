/* WHOLE-FILE CONVERSION of asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_a.s --
 * OvlFunc_956_2009474 is its only function, datacheck.py reports no data section, so
 * no split and no linker change.  533 instructions, 1432 bytes, 547 encodings and
 * 165 relocations identical.
 *
 * Path: 486 of 547 -> 142 -> 4 -> 0 in three edits.
 *
 * ================================================================
 * FAMILY-ORDERED PIN3 FILLS ARE THE WHOLE GAME AT THIS SIZE, AND THE q0 FILL IS
 * NOT OPTIONAL
 * ================================================================
 *
 * Two fill orders, read off the ROM PER CALLEE FAMILY:
 *   both args shifted (SetPos / SetSpeed):
 *       q1 = X; q2 = Y; q0 = S; q1 <<= a; q2 <<= b;
 *   arg1 shifted, arg2 plain (80921c4 / 809218c / 8092adc):
 *       q1 = X; q0 = S; q1 <<= a; q2 = Y;
 *
 * DROPPING THE q0 FILL FROM THE FIRST FORM -- leaving only the r1/r2 pins -- COSTS
 * 142 OF 547, because `mov r0,#N` then lands after both `lsl`s instead of between
 * the movs and the shifts.  That is the same mechanism as batch 281's
 * split-the-pinned-pair lever seen from the other side: what matters is where the
 * r0 fill sits RELATIVE to the mov/lsl pair, and both the split and the q0 fill are
 * ways of placing it.
 *
 * THE HImode INT-CARRIER RULE, a sixth instance, and the pool signal found it in
 * one look.  Four `*(short *)(p + 6) = 0;` stores pooled a `.word 0` and loaded it
 * with `ldrh`, whose pool_range is 64 -- which FORCED AN ENTIRE EXTRA EARLY POOL
 * DUMP at offset 0xb0 that the ROM does not have.  Routing through `int z = 0;`
 * removed the pool word and the dump together.  Reading the pool position before
 * any register, as the method says, is what made this a one-look diagnosis rather
 * than a register hunt.
 *
 * A REUSED POINTER IS THE DEFECT WHERE THE ROM USES `add r0, #N` IN PLACE.
 * Single-store actor sites cost two instructions each written as
 * `p = GetActor(n); p[0x23] = 2;` -- the named pointer buys a `mov rX, r0` copy.
 * `__MapActor_GetActor(n)[0x23] = 2;` is correct here.  The named pointer is right
 * ONLY where the ROM actually emits `mov r5, r0`.
 *
 * TWO `0x100`-SHAPED STORES IN TWO SIBLING FUNCTIONS WANT OPPOSITE SPELLINGS, AND
 * THE DISCRIMINATOR IS WHETHER THE VALUE LIVES.  Both this function and its
 * file-neighbour store to iwram_3001ebc + (0xe0 << 1).  Here the ROM builds 0x100
 * FRESH as `mov r5,#0x80 / lsl r5,#1` because the same value must survive to a
 * later __MapActor_Emote, and a NAMED `int h` local is actively harmful:
 * 142 -> 327 of 547.  In the neighbour the ROM re-uses the 0x1c0 offset register
 * (`sub r2,#0xc0`) and a bare `= 0x100` reproduces it.  BARE LITERAL WHEN THE VALUE
 * DIES AT THE STORE, and do not reach for a carrier because a sibling has one.
 *
 * Ships 34 of 38 pin blocks.  Twelve were inert SINGLY; the JOINT drop of all
 * twelve is +4 BYTES, and a greedy ladder accepted only four.  The joint drop was
 * never the sum of the singles in any of this batch's three ladders.
 *
 * No per-file Makefile rule matches this stem (rom_7e0928 appears nowhere in the
 * Makefile).
 */
extern int L5b80 __asm__(".L5b80");
extern unsigned char gScript_956__0200d668[];
extern unsigned char gScript_956__0200d738[];
extern unsigned char gScript_956__0200d808[];
extern unsigned char gScript_956__0200d8ac[];
extern unsigned char gScript_956__0200d950[];
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_956_20093c0(void);

extern void __DeleteFieldActor(int slot);
extern void __Func_807808c(int a);
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetExtra(int slot, int v);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *e, int f);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_809259c(int slot, int a);
extern void __Func_80925cc(int slot, int a);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_800c5b4(void);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_956_2009474(int a)
{
    unsigned char *p;
    int z;

    __DeleteFieldActor(0x27);
    __DeleteFieldActor(0x28);
    __Func_807808c(1);
    __PlaySound(0x11);
    __CutsceneStart();
    { PIN3; q1 = 0xc1; q2 = 0xc0; q0 = 8; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    if (a < 0)
        __MapActor_SetAnim(8, 0xa);
    else
        __MapActor_SetAnim(8, 8);
    __MapActor_SetBehavior(8, gScript_956__0200d668);
    __MapActor_SetPos(0, 0xbc << 19, 0xc0 << 16);
    p = __MapActor_GetActor(0);
    z = 0;
    *(short *)(p + 6) = z;
    __MapActor_SetBehavior(0, gScript_956__0200d738);
    __MapActor_SetAnim(0, 0x23);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 2; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 3; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xb7; q2 = 0xb8; q0 = 1; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xb7; q2 = 0xc8; q0 = 2; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xb5; q2 = 0xc0; q0 = 3; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    p = __MapActor_GetActor(1);
    *(short *)(p + 6) = z;
    p = __MapActor_GetActor(2);
    *(short *)(p + 6) = z;
    p = __MapActor_GetActor(3);
    *(short *)(p + 6) = z;
    __WaitFrames(1);
    __SetCameraTarget(0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
    __Func_8091200(0x10001, 1);
    __MapTransitionIn();
    __WaitMapTransition();
    __MessageID(0x20f1);
    __CutsceneWait(0x3c);
    __MapActor_SetBehavior(0, gScript_956__0200d950);
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x18) = 0x80 << 9;
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x1c) = 0x80 << 9;
    __MapActor_DoAnim(0, 0x24);
    p = __MapActor_GetActor(0);
    *(int *)(p + 8) += 0xc0 << 10;
    __CutsceneWait(0xa);
    p = __MapActor_GetActor(0);
    __Actor_SetSpriteFlags(p, 0);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0, gScript_956__0200d808);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xbc; q0 = 1; q1 <<= 3; q2 = 0xb0; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_Emote(1, 0x80 << 1, 0x14);
    __ActorMessage(2, 0);
    __MapActor_SetExtra(1, 2);
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0xba; q0 = 2; q1 <<= 3; q2 = 0xb0; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xbe; q0 = 1; q1 <<= 3; q2 = 0xb8; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xbc; q0 = 2; q1 <<= 3; q2 = 0xb0; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(1, 1);
    { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 2; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(2, 2);
    __CutsceneWait(0xa);
    __MapActor_SetExtra(2, 1);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x1e);
    __ActorMessage(3, 0);
    __MapActor_SetExtra(1, 3);
    __MapActor_SetExtra(2, 3);
    { PIN3; q1 = 0xba; q0 = 3; q1 <<= 3; q2 = 0xb8; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetExtra(2, 0);
    __MapActor_SetExtra(1, 0);
    { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 1; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetExtra(2, 1);
    __MapActor_SetExtra(1, 2);
    __CutsceneWait(0x28);
    __Func_80925cc(3, 2);
    __CutsceneWait(0xa);
    __MapActor_SetExtra(2, 3);
    __MapActor_SetExtra(1, 3);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __MapActor_SetBehavior(0, gScript_956__0200d8ac);
    __Func_80925cc(1, 2);
    __MapActor_SetExtra(1, 0);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __Func_80925cc(2, 2);
    __MapActor_SetExtra(2, 0);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __Func_80925cc(3, 2);
    __MapActor_SetExtra(3, 0);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __MapActor_SetBehavior(0, gScript_956__0200d950);
    __CutsceneWait(0x3c);
    L5b80 = 9;
    __StartTask(OvlFunc_956_20093c0, 0xc8 << 4);
    __CutsceneWait(5);
    __StopTask(OvlFunc_956_20093c0);
    __CutsceneWait(0x37);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_956_20093c0, q1); }
    __CutsceneWait(0x14);
    __StopTask(OvlFunc_956_20093c0);
    __CutsceneWait(0x28);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    L5b80 = 9;
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_956_20093c0, q1); }
    __CutsceneWait(0x23);
    __StopTask(OvlFunc_956_20093c0);
    __CutsceneWait(0x19);
    { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 3; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    L5b80 = 9;
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_956_20093c0, q1); }
    __CutsceneWait(0x23);
    __StopTask(OvlFunc_956_20093c0);
    __CutsceneWait(0x19);
    { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 2; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetExtra(3, 2);
    __MapActor_SetExtra(2, 3);
    __CutsceneWait(0x3c);
    __MapActor_SetExtra(3, 0);
    __MapActor_SetExtra(2, 0);
    L5b80 = 9;
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_956_20093c0, q1); }
    __CutsceneWait(0x23);
    __StopTask(OvlFunc_956_20093c0);
    __CutsceneWait(0x19);
    { PIN3; q1 = 0x84; q2 = 0x3c; q0 = 3; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(1, 3);
    __Func_809259c(2, 3);
    __Func_80925cc(3, 3);
    __MapActor_SetExtra(3, 2);
    __MapActor_SetExtra(1, 2);
    L5b80 = 9;
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_956_20093c0, q1); }
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_SetAnim(3, 3);
    __CutsceneWait(0x3c);
    __Func_809218c(3, 0xb7 << 3, 0xc8);
    __CutsceneWait(5);
    __Func_809218c(2, 0xab << 3, 0xb8);
    __CutsceneWait(3);
    { PIN3; q1 = 0xbd; q0 = 1; q1 <<= 3; q2 = 0xb8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xab; q0 = 1; q1 <<= 3; q2 = 0xb8; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(3);
    __MapActor_SetAnim(3, 1);
    __MapActor_SetExtra(3, 0);
    __CutsceneWait(0x3c);
    { PIN3; q1 = 0xb3; q0 = 3; q1 <<= 3; q2 = 0xc8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xab; q0 = 3; q1 <<= 3; q2 = 0xb8; __Func_809218c(q0, q1, q2); }
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0xbd; q2 = 0xb0; q0 = 1; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xb7; q2 = 0xc0; q0 = 2; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc3; q2 = 0xc8; q0 = 3; q1 <<= 19; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    __Func_800c5b4();
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(1);
    __MessageID(0x214c);
    __ActorMessage(1, 0);
    __ActorMessage(2, 0);
    __ActorMessage(3, 0);
    __CutsceneWait(0x3c);
    __CutsceneEnd();
}
