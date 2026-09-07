// fakematch
/* ovl_30_c_c_c_c_a_a_a_c_c_c_a.c  --  OvlFunc_882_200a180 + OvlFunc_882_200a8a4
 *   [the WHOLE of asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_a.s --
 *    both `.thumb_func_start`s, so NO SPLIT is required and overlay.ld:55 stays
 *    verbatim]
 *
 *   OK WHOLE TU -- 2984 bytes, 1137 encodings and 317 relocations identical
 *
 * Two straight-line cutscene scripts, 705 + 432 instructions, 184 + 120 call
 * sites over 40 distinct callees.  FAKEMATCH: matched with the register-pin
 * idiom, so both names go in fakematch.txt.
 *
 * WHAT THE PINS ARE FOR.  Unpinned, gcc commons the repeated shifted constants
 * into r8-r11 and the prologue reads `push {r5, r6, r7, lr}` + four high-register
 * saves against the ROM's `push {r5, r6, lr}` -- 663 differing.  58 uniform
 * ASCENDING pins in the first function and 24 in the second (every one nominated
 * by the recorded CSE rule, plus five ordering-only extras in the second) rebuild
 * each constant at its own call site.  Every pin is `q0 = ...; q1 = ...; q2 = ...`
 * IN ARGUMENT ORDER: the ROM's own emitted register order was transcribed
 * verbatim at the eight sites that looked as if they needed it and measured
 * EXACTLY EQUAL to the plain ascending fill at all eight.  Only ONE site in the
 * whole file wants a non-ascending fill -- site 92 of the second function,
 * `__MapActor_Emote(9, 0x80 << 1, 0x1e)`, which needs 1|2|0.
 *
 * THREE THINGS THAT ARE NOT PINS, and each was worth more than any pin:
 *
 *  1. THE FLAG ID PINNED TO r0 AT BOTH ENDS.  `p0 = 0x83a` before __GetFlag and
 *     again before __SetFlag.  Without it rerun-CSE keeps 0x83a in r7 and adds a
 *     push the ROM does not have -- the recorded "an added push holding a
 *     commoned constant is a FLAG tell", cured here in the C rather than with
 *     CSE_CFLAGS, exactly as the sibling ovl_30_c_c_c_a_a_c_a_a.c does it.
 *
 *  2. THE SCRIPT POINTER'S THREE CSE-NOMINATED SITES MUST NOT BE PINNED.  The
 *     ROM loads gScript_882__0200cec8 ONCE into r6 and copies it (`mov r1, r6`)
 *     at each __MapActor_SetBehavior.  Pinning sites 9/18/25 rematerialises it
 *     three times: 507 differing against 34.  This is the template's recorded
 *     failure mode -- a symbol address the ROM loads once and copies.
 *
 *  3. ONE LOCAL PER BLOCK SHAPE, NOT ONE POINTER FOR THE FUNCTION.  Three
 *     `unsigned char *` locals: `r` for the GetActor/Random/strh blocks (it must
 *     survive two calls), `t` for the two-store f18/f1c blocks, `p` for the
 *     guarded GetActor/SetPos.  Sharing one pointer costs `mov r6, r0` at the
 *     guard, where the ROM uses r0 directly: 663 -> 34 came mostly from this.
 *     `r` additionally needs `register ... __asm__("r5")`: with it free, gcc puts
 *     the script in r5 and the actor pointer in r6, the ROM's assignment
 *     reversed, for 15 differing that no declaration order reaches.
 *
 * THE LAST TWO INSTRUCTIONS IN THE FIRST FUNCTION NEEDED THE STRUCT MEMBER.
 * The four bit sites are `&= 0xfe` twice then `|= 1` twice on the byte at +0x5a
 * of two actors, each mask commoned into r5 across a call.  Through
 * `unsigned char *` gcc emits the ROM's `and r3, r2` / `and r5, r3` / `orr r3, r5`
 * -- and then `orr r3, r3, r5` where the ROM has `orr r5, r3`.  Reaching the byte
 * as an aggregate member instead is exact.  That confirms the recorded batch-240
 * discriminator ("AGGREGATE MEMBER REFERENCE versus dereferenced pointer") on the
 * same `->f5a |= 1` shape, and the RTL dumps say why the pair splits: the Thumb
 * `andsi3` expander force_reg's its second operand into a fresh SImode pseudo
 * while `iorsi3` leaves a `subreg:SI (reg:QI ...)`, and only the plain-REG form
 * gets the tie fixed up, so `&=` matches unaided and `|=` does not.  Measured on
 * a four-function probe: `&=` pair 2nd site `and r5, r5, r3` (right), `|=` and
 * `^=` pairs `orr/eor r3, r3, r5` (wrong), struct member `orr r5, r5, r3`
 * (right).  Ruled out first, all inert or worse: `= 1 | x`, an `int` mask local,
 * an `unsigned char` mask local, naming the pointer, and an opaque-1 asm.
 * THE STRUCT IS FOR f5a ONLY -- spelling f8/f10/f18/f1c/f28/f64 through the same
 * struct is 642 differing.
 *
 * THE SECOND FUNCTION NEEDS TWO WALLS AND THE MESSAGE-BASE PIN.  `m` is
 * `register int __asm__("r5")` holding 0xe9b and later 0xea1, read back as
 * `m + 4`, `m + 5`, `m + 1`, `m + 2` inside two `__Func_8091c7c(0,0)` diamonds;
 * without the pin cprop folds every use into its own pool word (230 differing).
 * With it, `ldr r5, =0xe9b` is scheduled THREE statements early, and ONE
 * `do { } while (0);` in front of each `m = ...;` is exact -- one wall per site,
 * not one per statement crossed; two and three walls measure the same.
 * Total barrier budget: 2.
 *
 * The `.L` operands of __Func_8010560 are the four shared data labels, declared
 * with the asm-name trick already used by ovl_30_c_c_c_a_c_c_c_a_c_c_b.c in this
 * directory.  All four are `.global` in
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_c_c.s, which also defines seven of
 * the eight gScript symbols (gScript_881__0200ca78 comes from rom_77a7c8).
 *
 * PIN MINIMISATION ran to a fixpoint FROM BOTH ENDS on both functions and both
 * directions survive the same sets: 58 of 58 required in the first (the 5 inert
 * CSE nominations 81/93/103/120/160 are already out), 24 of 28 in the second
 * (74, 83, 98, 108 dropped).  Every single-pin drop lands either at 2-3 differing
 * with RELOCATIONS silent (an ordering pin) or at 14+ with RELOCATIONS differing
 * (a CSE pin); nothing landed in 4-13, which is the recorded "no middle" band
 * holding on 82 more drops.
 *
 * Harness: scratch_elev/b252/big/ -- extract.py, mkbody.py, gen.py, minimise.py,
 * combine.py, objcmp_all.py (whole-TU objcmp, since --func filters only the
 * reference), run.sh; f2/ holds the same chain for the second function.
 */
struct Actor {
    unsigned char pad00[0x5a];
    unsigned char f5a;
};

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_809202c(void);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_882_200a8a4(void);
extern unsigned char gScript_882__0200cec8[];
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_RunScript(int slot, void *s);
extern void __Func_8010560(void *p, int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern unsigned char gScript_881__0200ca78[];
extern unsigned char gScript_882__0200cab4[];
extern unsigned char gScript_882__0200cb28[];
extern unsigned char gScript_882__0200cb9c[];
extern unsigned char gScript_882__0200cc0c[];
extern unsigned char gScript_882__0200cc5c[];
extern unsigned char gScript_882__0200cca8[];
extern unsigned char L578a[] __asm__(".L578a");
extern unsigned char L57a0[] __asm__(".L57a0");
extern unsigned char L57cc[] __asm__(".L57cc");
extern unsigned char L57e2[] __asm__(".L57e2");

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_882_200a180(void)
{
    unsigned char *p;
    register unsigned char *r __asm__("r5");
    unsigned char *t;
    register int p0 __asm__("r0");
    int w;
    int n;

    p0 = 0x83a;
    if (__GetFlag(p0) != 0)
        return;
    __CutsceneStart();
    { PIN3; q0 = 0xa; q1 = 0xc0 << 16; q2 = 0x4be0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 0x5);
    r = __MapActor_GetActor(0xa);
    n = __Random() % 0x5a + 0x3c;
    r += 0x64;
    *(short *)r = n;
    __MapActor_SetBehavior(0xa, gScript_882__0200cec8);
    { PIN3; q0 = 0x9; q1 = 0xc0 << 16; q2 = 0x4a50000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0xe3 << 16; q2 = 0x4be0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x18, 0x6);
    r = __MapActor_GetActor(0x18);
    n = __Random() % 0x5a + 0x3c;
    r += 0x64;
    *(short *)r = n;
    __MapActor_SetBehavior(0x18, gScript_882__0200cec8);
    { PIN3; q0 = 0x19; q1 = 0xfa << 16; q2 = 0x4be0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x19, 0x6);
    r = __MapActor_GetActor(0x19);
    n = __Random() % 0x5a + 0x3c;
    r += 0x64;
    *(short *)r = n;
    __MapActor_SetBehavior(0x19, gScript_882__0200cec8);
    { PIN3; q0 = 0x1a; q1 = 0xe3 << 16; q2 = 0x4a50000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x1a; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x17; q1 = 0xf3 << 16; q2 = 0x4fd0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x17; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __WaitFrames(0x3);
    __MessageID(0xe8c);
    __ActorMessage(0x201a, 0x0);
    { PIN3; q0 = 0x0; q1 = 0x80 << 1; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x96; q2 = 0x446;
      __Func_80921c4(q0, q1, q2); }
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x16, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q0 = 0x16; q1 = 0x84; q2 = 0x446;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092848(0x0, 0x16, 0x0);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x0; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x80 << 11; q1 = 0x80 << 8;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xd8 << 16; q1 = -0x1; q2 = 0x9a << 19; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    __Func_80925cc(0xa, 0x2);
    __Func_8093040(0xa, 0x0, 0xa);
    __Func_80925cc(0x17, 0x3);
    __Func_8092adc(0x9, 0x0, 0xa);
    __MapActor_DoAnim(0x9, 0x3);
    __Func_8093040(0x9, 0x0, 0xa);
    { PIN3; q0 = 0x9; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xe8 << 16; q1 = -0x1; q2 = 0x4e50000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __PlaySound(0x86);
    __MapActor_Jump(0x17, 0x4, 0x0);
    __MapActor_SetAnim(0x17, 0x6);
    __CutsceneWait(0xa);
    __MapActor_SetPos(0x17, 0x0, 0x0);
    __CutsceneWait(0x3c);
    __Func_809202c();
    __MapActor_SetAnim(0xa, 0x1);
    w = 0x80 << 9;
    t = __MapActor_GetActor(0xa);
    *(int *)(t + 0x18) = w;
    *(int *)(t + 0x1c) = w;
    __MapActor_SetAnim(0x18, 0x1);
    t = __MapActor_GetActor(0x18);
    *(int *)(t + 0x18) = w;
    *(int *)(t + 0x1c) = w;
    __MapActor_SetAnim(0x19, 0x1);
    t = __MapActor_GetActor(0x19);
    *(int *)(t + 0x18) = w;
    *(int *)(t + 0x1c) = w;
    __Func_809259c(0xa, 0x2);
    __Func_809259c(0x9, 0x2);
    __Func_809259c(0x18, 0x2);
    __Func_809259c(0x19, 0x2);
    __Func_80925cc(0x1a, 0x2);
    { PIN2; q0 = 0x9999; q1 = 0x1333;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xd8 << 16; q1 = -0x1; q2 = 0x9a << 19; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN2; q0 = 0x1a; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x9, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_80925cc(0x1a, 0x2);
    __Func_809259c(0x1a, 0x3);
    __ActorMessage(0x1a, 0x0);
    __MapActor_Jump(0x19, 0x2, 0x0);
    { PIN3; q0 = 0x19; q1 = 0xea; q2 = 0x4b5;
      __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_Jump(0x1a, 0x2, 0x0);
    { PIN3; q0 = 0x1a; q1 = 0xe3; q2 = 0x4b1;
      __MapActor_TravelTo(q0, q1, q2); }
    __CutsceneWait(0x5a);
    { PIN4; q0 = 0xe8 << 16; q1 = -0x1; q2 = 0x4e50000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __MapActor_SetPos(0x17, 0xf3 << 16, 0x4fd0000);
    __WaitFrames(0x1);
    __PlaySound(0x6a);
    *(int *)(__MapActor_GetActor(0x17) + 0x28) = 0x80 << 10;
    __CutsceneWait(0x6);
    __MapActor_SetAnim(0x17, 0x7);
    __CutsceneWait(0x14);
    __Func_809202c();
    __CutsceneWait(0x14);
    __Func_80933d4(0x19999, 0x3333);
    __Func_80933f8(0xd8 << 16, -0x1, 0x9a << 19, 0x1);
    __Func_8093530();
    __Func_80925cc(0x18, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x18; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092848(0x18, 0xa, 0x0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 0x2);
    { PIN2; q0 = 0x800a; q1 = 0x0;
      __ActorMessage(q0, q1); }
    ((struct Actor *)__MapActor_GetActor(0x19))->f5a &= 0xfe;
    ((struct Actor *)__MapActor_GetActor(0x1a))->f5a &= 0xfe;
    { PIN3; q0 = 0x19; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1a; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0xf7; q2 = 0x4ba;
      __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q0 = 0x1a; q1 = 0xe3; q2 = 0x4a5;
      __Func_8092158(q0, q1, q2); }
    ((struct Actor *)__MapActor_GetActor(0x19))->f5a |= 1;
    ((struct Actor *)__MapActor_GetActor(0x1a))->f5a |= 1;
    __Func_8092adc(0x1a, 0xc0 << 7, 0x0);
    { PIN3; q0 = 0x19; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x18, 0x4);
    __Func_8093040(0x8018, 0x0, 0xa);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xa, 0x0, 0xa);
    __MapActor_SetAnim(0xa, 0x4);
    { PIN3; q0 = 0x800a; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x83 << 1; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x9, 0x0, 0x1e);
    { PIN3; q0 = 0x9; q1 = 0x80 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x9, 0x0, 0xa);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x90 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1a; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xa, 0x1);
    { PIN3; q0 = 0x800a; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(0x9, 0x4);
    __Func_8093040(0x9, 0x0, 0xa);
    { PIN3; q0 = 0xa; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x105; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1a; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0x9, 0x0, 0xa);
    __Func_8092848(0x18, 0x19, 0x0);
    __CutsceneWait(0x14);
    __Func_8092adc(0x9, 0x0, 0x0);
    __Func_8092adc(0xa, 0x0, 0xa);
    { PIN3; q0 = 0x18; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x18, 0x3);
    __MapActor_DoAnim(0x19, 0x3);
    __Func_8092848(0xa, 0x9, 0x0);
    __CutsceneWait(0x14);
    __Func_80925cc(0xa, 0x1);
    __Func_8093040(0x800a, 0x0, 0xa);
    __MapActor_DoAnim(0x9, 0x3);
    { PIN3; q0 = 0x18; q1 = 0xd0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x18, 0x1);
    __Func_8093040(0x18, 0x0, 0xa);
    __Func_8092adc(0xa, 0x0, 0x0);
    __Func_8092adc(0x9, 0x0, 0x0);
    __Func_80925cc(0x1a, 0x1);
    { PIN3; q0 = 0x1a; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x19, 0x3);
    __Func_8093040(0x19, 0x0, 0xa);
    __MapActor_DoAnim(0x1a, 0x3);
    __CutsceneWait(0x14);
    __Func_80925cc(0x9, 0x2);
    __MapActor_DoAnim(0x9, 0x3);
    __Func_8093040(0x9, 0x0, 0xa);
    { PIN3; q0 = 0x1a; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x1a, 0x3);
    __CutsceneWait(0x14);
    __Func_80925cc(0x9, 0x1);
    __Func_8093040(0x9, 0x0, 0xa);
    OvlFunc_882_200a8a4();
    p0 = 0x83a;
    __SetFlag(p0);
    __CutsceneEnd();
}

void OvlFunc_882_200a8a4(void)
{
    unsigned char *p;
    unsigned char *r;
    register int m __asm__("r5");

    { PIN3; q0 = 0x1a; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0xb0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x1a, 0x3);
    __MapActor_SetAnim(0x18, 0x3);
    __MapActor_SetAnim(0x19, 0x3);
    __MapActor_SetAnim(0x9, 0x3);
    __MapActor_DoAnim(0x19, 0x3);
    __CutsceneWait(0x14);
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    { PIN4; q0 = 0x86 << 16; q1 = (int)-0x1; q2 = 0x4ab0000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 0x1a; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0x1a, gScript_882__0200cab4);
    __MapActor_RunScript(0x9, gScript_881__0200ca78);
    __PlaySound(0x9e);
    __Func_8010560(L57a0, 0x26, 0x48);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x9; q1 = 0x95; q2 = 0x497;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0x9, 0x0, 0x0);
    { PIN3; q0 = 0x19; q1 = 0xfa; q2 = 0x4be;
      __Func_80921c4(q0, q1, q2); }
    __Func_809202c();
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 0x5);
    __MapActor_SetAnim(0x18, 0x6);
    __MapActor_SetAnim(0x19, 0x6);
    r = __MapActor_GetActor(0xa);
    r += 0x64;
    *(short *)r = __Random() % 0x5a + 0x3c;
    r = __MapActor_GetActor(0x18);
    r += 0x64;
    *(short *)r = __Random() % 0x5a + 0x3c;
    r = __MapActor_GetActor(0x19);
    r += 0x64;
    *(short *)r = __Random() % 0x5a + 0x3c;
    __MapActor_SetBehavior(0xa, gScript_882__0200cec8);
    __MapActor_SetBehavior(0x18, gScript_882__0200cec8);
    __MapActor_SetBehavior(0x19, gScript_882__0200cec8);
    __MapActor_WaitScript(0x1a);
    __CutsceneWait(0xa);
    __PlaySound(0x9f);
    __Func_8010560(L57e2, 0x26, 0x48);
    __CutsceneWait(0x1e);
    __Func_809202c();
    { PIN4; q0 = 0xe0 << 15; q1 = (int)-0x1; q2 = 0x4c90000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __PlaySound(0x9e);
    __Func_8010560(L578a, 0x23, 0x49);
    __CutsceneWait(0x14);
    __Func_809202c();
    __MapActor_SetBehavior(0x9, gScript_882__0200cb28);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0x1a, gScript_882__0200cb9c);
    __CutsceneWait(0x28);
    __PlaySound(0x9f);
    __Func_8010560(L57cc, 0x23, 0x49);
    __MapActor_WaitScript(0x1a);
    __Func_809202c();
    __CutsceneWait(0x28);
    do { } while (0);
    m = 0xe9b;
    __MessageID(m);
    __Func_8093040(0x9, 0x0, 0x14);
    __MapActor_DoAnim(0x1a, 0x3);
    __Func_8093040(0x201a, 0x0, 0x28);
    __MapActor_SetAnim(0x9, 0x3);
    __MapActor_DoAnim(0x1a, 0x3);
    __CutsceneWait(0x1e);
    __MapActor_SetBehavior(0x9, gScript_882__0200cc0c);
    __MapActor_SetBehavior(0x1a, gScript_882__0200cc5c);
    __CutsceneWait(0x28);
    __Func_80933d4(0x80 << 10, 0x80 << 7);
    __Func_80933f8(0xd2 << 15, -0x1, 0x43e0000, 0x1);
    __MapActor_WaitScript(0x9);
    __Func_8092adc(0x9, 0x0, 0x0);
    { PIN3; q0 = 0x9; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x9, 0x0, 0xa);
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80921c4(0x9, 0x69, 0x43e);
    __Func_80925cc(0x9, 0x2);
    __Func_8092c40(0x8009, 0x0);
    __Func_8092adc(0x16, 0x0, 0x0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(0x9, 0x3);
        __MessageID(m + 4);
    } else {
        __Func_80925cc(0x9, 0x2);
        __MessageID(m + 5);
    }
    { PIN2; q0 = 0x8009; q1 = 0x0;
      __ActorMessage(q0, q1); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80 << 1; q2 = 0x1e; q0 = 0x9;
      __MapActor_Emote(q0, q1, q2); }
    do { } while (0);
    m = 0xea1;
    __MessageID(m);
    { PIN2; q0 = 0x8009; q1 = 0x0;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(0x9, 0x3);
        __MessageID(m + 1);
        __Func_8093040(0x8009, 0x0, 0x1e);
        { PIN3; q0 = 0x16; q1 = 0x80 << 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_SetAnim(0x0, 0x3);
        __MapActor_SetAnim(0x16, 0x3);
        __MapActor_DoAnim(0x9, 0x3);
        __CutsceneWait(0x28);
    } else {
        { PIN3; q0 = 0x9; q1 = 0x105; q2 = 0x5a;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x9; q1 = 0x103; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_SetAnim(0x9, 0x4);
        __MessageID(m + 2);
        __ActorMessage(0x8009, 0x0);
    }
    __MapActor_SetBehavior(0x9, gScript_882__0200cca8);
    __CutsceneWait(0x5a);
    __Func_8092848(0x0, 0x16, 0x0);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x16, 0x2);
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_TravelTo(0x16, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x16);
    __MapActor_SetPos(0x16, 0x0, 0x0);
}
