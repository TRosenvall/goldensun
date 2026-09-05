// fakematch
/* OvlFunc_950_200813c  --  0x0200813c
 *   [asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_a.s, 2nd of 2]
 *
 * 180 instructions of straight-line cutscene -- ONE basic block, no branch
 * anywhere.  Byte-exact: 492 bytes, 186 encodings and 54 relocations
 * identical under tools/objcmp.py.
 *
 * THE PROLOGUE PICKS THE CURE, AND HERE IT SAYS "PIN".  `push {r5, lr}` saves
 * exactly one callee-saved register and r5 holds the __CreateActor RESULT
 * (`mov r5, r0`, re-read at Actor_SetAnim x2 and DeleteActor) -- a base
 * pointer, not a constant.  So the ROM keeps NO constant across the body and
 * every repeated value is rebuilt at each use.  Plain C is 185 lines against
 * 180, 179 differing, with `push {r5, r6, lr}` + `mov r6, r8` / `push {r6}`
 * and a matching r8 epilogue: cse_main commons 0x80 << 8 (2 sites),
 * 0xd4 << 2 (2) and 0xc0 << 8 (2), and because every use straddles a `bl` the
 * pseudos must be callee-saved.  IT READS AS A LENGTH DIFFERENCE FIRST.
 *
 * THE NAMED-LOCAL CURE IS WORSE, as the prologue predicts: one `int` per
 * duplicated value assigned at the top measures 188 lines and 180 differing
 * against plain C's 185/179.  A named local buys the value a pseudo
 * local-alloc is happy to keep live, which is the opposite of what is wanted.
 *
 * NINE PINS, at a greedy fixpoint over 11 candidate sites.  Two are inert one
 * at a time -- the SECOND `__Func_8092adc(0x19, 0xc0 << 8, 0)` and
 * `__Func_8092adc(0, 0x80 << 8, 0)` -- they are jointly removable, and a
 * second one-at-a-time round over the surviving nine finds no new inert pin.
 * Both dropped pins are the LAST use of their value; the pin at the FIRST use
 * already leaves no pseudo for CSE to hand the later site.  Verified as a set
 * under objcmp, not by line count.
 *
 * THE NINE SPLIT INTO TWO JOBS, and the drop cost says which is which:
 *   CSE-destruction -- SetSpeed(0x80<<9, 0x80<<8) 124 differing when dropped,
 *     Func_80921c4(0x96<<2, 0xd4<<2) 27, the first Func_8092adc(0xc0<<8) 29.
 *   Ordering only -- the two Emotes (2 each), Func_8092304(0,0,-0x10) (2), the
 *     second Func_80921c4 (3) and both pooled SetSpeeds (3 each).
 * Note the second `0xd4 << 2` site IS load-bearing where the second `0xc0 << 8`
 * site is not: with no branch in the function the first-use rule is not about
 * block structure here, it is about what each pin is doing.
 *
 * THE FILLS ARE UNIFORM, NOT TRANSCRIBED, and there is no residue to
 * transcribe.  Every pinned site is one statement per argument, ascending
 * q0..q3, whole value per statement, even where the ROM emits the group
 * differently -- `mov r1 / mov r0 / lsl r1 / mov r2` at the first
 * __MapActor_Emote, `mov r1 / mov r2 / mov r0 / lsl r1` at the second,
 * `mov r2 / mov r1 / neg r2 / mov r0` at __Func_8092304.  sched2 reproduces
 * all three.  Transcribing either Emote per-instruction is BYTE-IDENTICAL, so
 * the uniform form is kept.
 *
 * THE POOLED VALUES ARE LITERALS, NOT SYMBOLS.  0x2394, 0x101, 0x16666/0xb333
 * and 0x1cccc/0xe666 are spelled as bare ints and objcmp reports 54
 * relocations identical; the reference carries no R_ARM_ABS32 in this
 * function's range beyond the `bl` targets.  const.sym's halfword exception
 * does not apply -- none of these values meets a halfword.
 *
 * NO TWIN.  tools/fuzzy_solved.py --min-ratio 0.40 does not list this
 * function at all, so there is no solved near-twin to copy from.
 *
 * No Makefile rule names this path -- rom_7d5838 has exactly one explicit rule
 * and it is for ovl_30_c_c_a_c_c_c (CSE_CFLAGS), with no wildcard -- so the
 * tree default -O2 applies; objcmp against the original asm/ path and a
 * scratch copy of the reference agree, which is the wildcard check.  tryc
 * freshness confirmed with -fno-omit-frame-pointer as a positive control
 * (echoed, 182 lines / 182 differing -- the probe moves).
 *
 * LANDING NEEDS A SPLIT.  The .s holds two functions and this is the SECOND;
 * overlays/rom_7d5838/overlay.ld:23 names the single .o.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern unsigned char *__CreateActor(int id, int x, int y, int z);
extern void __DeleteActor(unsigned char *a);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __ActorMessage(int slot, int a);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_950_200813c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __MessageID(0x2394);
    __CutsceneWait(0x28);
    p = __CreateActor(0x8e << 1, 0x96 << 18, 0, 0xce << 18);
    __Actor_SetSpriteFlags(p, 0);
    __Actor_SetAnim(p, 6);
    __CutsceneWait(0xa);
    __Actor_SetAnim(p, 1);
    __CutsceneWait(0x28);
    __DeleteActor(p);
    __CutsceneWait(2);
    { PIN3; q0 = 0x19; q1 = 0x80 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x96 << 2; q2 = 0xd4 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __ActorMessage(0x19, 0);
    __Func_80925cc(0x19, 2);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x19; q1 = 0x8e << 2; q2 = 0xd4 << 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x19, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_Emote(0x19, 0x84 << 1, 0x32);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0; q2 = -0x10;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_8092adc(0x19, 0xc0 << 6, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(0x19, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x19, 0);
    __CutsceneWait(0x14);
    __MapActor_Emote(0, 0x101, 0x32);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x19, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x19, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x19; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x19, 0);
    { PIN3; q0 = 0x19; q1 = 0x16666; q2 = 0xb333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x19, 0x10, 0);
    __Func_8092304(0x19, 0, 0x20);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x19, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0x19, 0);
    __Func_8092304(0, 0x10, 0);
    __Func_8092adc(0, 0x80 << 8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x19; q1 = 0x1cccc; q2 = 0xe666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x19, 0, 0x30);
    __MapActor_SetPos(0x19, 0, 0);
    __CutsceneEnd();
}
