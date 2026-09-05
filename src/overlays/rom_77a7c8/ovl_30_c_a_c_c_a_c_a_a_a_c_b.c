// fakematch
/* OvlFunc_881_200a274  --  0x0200a274
 *
 * From goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c.s, the
 * SECOND of three functions in the TU.  177 instructions of straight-line
 * cutscene: fetch actor 0xa, poke three of its fields, transition in, walk
 * actor 0xa and actor 0 through a chase, transition out.
 * Byte-exact: 564 bytes, 205 encodings and 50 relocations identical.
 *
 * CONSTANT-CSE, AND THE PROLOGUE PICKS THE CURE.  Plain C is 197 lines against
 * the ROM's 177 with 191 differing and a `push {r5, r6, r7, lr}` + r8-r11
 * prologue: nine pool constants (-1, 0x858, 0x878, 0x838, 0x6666, 0x121,
 * 0x15f8, 0x8580000, 0x15d80000) are each used at two to four call sites on one
 * straight-line path, cse_main commons them, and the pseudos have to be
 * callee-saved because the uses straddle calls.  The ROM's prologue is
 * `push {r5, lr}` and the one register it keeps holds the ACTOR POINTER, not a
 * constant -- so the ROM rebuilds every constant at every use and only PINS
 * reach it.  The named-local cure would be wrong here for the same reason.
 *
 * TWENTY-TWO PINS, at a greedy fixpoint over 35 candidate sites.  Stripping one
 * at a time gives THIRTEEN individually inert; unlike the rom_7d95dc siblings
 * all thirteen are also jointly removable, and a second one-at-a-time round
 * over the survivors finds no new inert pin.  Verified as a set under objcmp,
 * not just by line count.
 *
 * THE PIN SET IS SMALLER THAN THE REPEAT COUNT BECAUSE ONE PIN AT THE FIRST USE
 * COVERS THE LATER ONES.  0x121 is used twice: pinning the first `__PlaySound`
 * is worth 110 differing, and with it pinned the second is inert.  Same for the
 * two `__Func_80933f8` pool sites (first load-bearing, second inert) and for
 * `__MapActor_SetPos`/`__Func_80921c4` at the tail.  `0x8d` is a bare `mov` at
 * both its sites, never commoned, and neither `__PlaySound(0x8d)` needs a pin.
 *
 * DO NOT TRANSCRIBE THE ROM'S EMITTED ORDER -- MEASURED, NOT ASSUMED.  Fourteen
 * of the pinned sites emit their argument fill in a NON-ascending order in the
 * ROM (`ldr r2 / mov r0 / ldr r1` at three `__Func_8092158` sites, `mov r1 /
 * mov r0 / lsl r1 / mov r2` at all three `__Func_8092adc` sites, and so on).
 * Writing every pinned fill UNIFORMLY -- one statement per argument, ascending
 * q0, q1, q2, q3, whole value per statement -- is byte-identical to
 * transcribing each site's own order.  sched2 produces all fourteen transposed
 * orders from the uniform spelling.  This extends `OvlFunc_953_200a668`'s
 * five-site result to a whole function and to a second shape (pool loads, not
 * just mov/lsl pairs): the uniform form is the one to write.
 *
 * THE ONE PLACE PLAIN C IS WRONG OUTSIDE THE FILLS is the halfword store.
 * `*(short *)(a + 6) = 0x80 << 7;` folds 0x4000 into the literal pool and emits
 * one `ldr`, where the ROM has `mov r3, #0x80 / lsl r3, #7`.  Naming it makes
 * it cheap enough for local-alloc to rematerialise as mov+lsl -- the same
 * polarity as the loop-invariant rule, in the direction that WANTS a name.
 * Worth 153 differing.  The two word stores above it need no such help: they
 * share one value, so CSE gives them a pseudo and rematerialisation follows.
 * Naming that shared value too is byte-identical and is left out.
 *
 * No Makefile wildcard captures this object; tree default -O2.  Checked with
 * `-fno-omit-frame-pointer` as a positive control that tryc honours and echoes
 * bare -f flags (179 lines, 178 differing -- the probe moves).
 */
extern unsigned char *__MapActor_GetActor(int slot);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_881_200a274(void)
{
    unsigned char *a = __MapActor_GetActor(0xa);

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __WaitFrames(1);
    *(int *)(a + 0x18) = 0xc0 << 9;
    *(int *)(a + 0x1c) = 0xc0 << 9;
    { int v = 0x80 << 7; *(short *)(a + 6) = v; }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __MapActor_SetPos(0xa, 0x1568 << 16, 0x838 << 16);
    __WaitFrames(1);
    __PlaySound(0x8d);
    { PIN3; q0 = 0xa; q1 = 0x19999; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 2);
    { PIN3; q0 = 0xa; q1 = 0x156d; q2 = 0x858; __Func_8092158(q0, q1, q2); }
    { PIN2; q0 = 0x6666; q1 = 0xccc; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x15b8 << 16; q1 = -1; q2 = 0x858 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0xa; q1 = 0x159e; q2 = 0x858; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x15a8; q2 = 0x86e; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x15e8; q2 = 0x878; __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 1);
    { PIN1; q0 = 0x121; __PlaySound(q0); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x15d8 << 16; q2 = 0x878 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    __MapActor_Jump(0, 6, 0);
    { PIN3; q0 = 0; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x15c8; q2 = 0x878; __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0, 0x28);
    __PlaySound(0x8d);
    __MapActor_SetAnim(0xa, 2);
    { PIN3; q0 = 0xa; q1 = 0x15f8; q2 = 0x878; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x15f8; q2 = 0x838; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x15bd; q2 = 0x838; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x15b8; q2 = 0x853; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1572; q2 = 0x858; __Func_8092158(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1568; q2 = 0x838; __Func_8092158(q0, q1, q2); }
    __MapActor_SetPos(0xa, 0, 0);
    __PlaySound(0x121);
    __CutsceneWait(0x28);
    __Func_80933f8(0x15d8 << 16, -1, 0x858 << 16, 1);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0x15d8, 0x858);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x14);
    __CutsceneEnd();
}
