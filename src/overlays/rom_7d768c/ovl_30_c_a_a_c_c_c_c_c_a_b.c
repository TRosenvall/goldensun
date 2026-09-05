// fakematch
/* OvlFunc_952_2008ff8  --  0x02008ff8
 *   [asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a.s, 2nd of 2]
 *
 * 744 instructions of pure straight-line cutscene script -- no loop, no
 * conditional, no label except the mid-body pool jump.  Byte-exact: 2032
 * bytes, 756 encodings and 250 relocations identical (tools/objcmp.py).
 * tryc's OK is WEAK here: the reference keeps its literal pool INSIDE the
 * function (`.pool_aligned` at ref line 921), so tryc warns and normalises
 * pool loads to `=value`; only objcmp settles it.
 *
 * ONE LEVER, 37 PINS.  `push {lr}` -- no callee-saved register at all, so the
 * ROM keeps NO constant anywhere and rebuilds every repeated one at every use.
 * Plain C is 750 lines against 745 with 669 differing, and the extra five are
 * the tell: `push {r5, r6, r7, lr}` plus four `mov rN, r8..r11` spills, i.e.
 * cse_main commoned seven pseudos (0x80<<9, 0x80<<8, 0x80<<10, 0x81<<1,
 * 0x101, 0x105, -0x10) that all had to be callee-saved because every use
 * straddles a `bl`.  Pinning the call sites destroys the CSE and the residue
 * -- which was homogeneous, all argument-order transposition -- goes with it.
 *
 * THE FILLS ARE UNIFORM, NOT TRANSCRIBED.  Every pinned site is one statement
 * per argument, ascending q0..q3, whole value per statement, even where the
 * ROM emits the site differently -- `mov r1 / mov r2 / mov r0 / lsl r1 / lsl
 * r2` at __MapActor_SetSpeed, `mov r2 / mov r0 / ldr r1` at several
 * __MapActor_Emote sites, `mov r1 / mov r2 / neg r1 / mov r0` at
 * __Func_8092304(0x14, -0x10, 0), `mov r1 / lsl r1 / mov r2 / mov r0` at
 * __Func_8092adc(0x16, 0x80<<7, 0).  sched2 reproduces all of them from the
 * one spelling.  NO SITE WANTS THE DESCENDING FILL: descending everywhere
 * costs 88 differing and descending at the first __MapActor_SetSpeed alone
 * costs 4.  (__Func_8092c40, the documented descending exception, is not
 * called here.)
 *
 * THIRTY-SEVEN PINS from 43 candidate sites, at a greedy fixpoint.  All 43
 * expensive-constant sites were pinned first and that also matches byte for
 * byte.  Greedy removal, re-tested under objcmp after every drop, finds SIX
 * inert: __SetFlag(0x96b), __MessageID(0x2021), __Func_80933f8, the
 * __Func_80921c4(0x16, ...) site, __Func_8092adc(0x16, 0x80<<7, 0) and the
 * SECOND __Func_8092304(0x14, -0x10, 0).  A second one-at-a-time round over
 * the surviving 37 finds every one load-bearing, verified under objcmp in this
 * file's final shape.  Note the polarity split on the repeated pairs: the
 * FIRST __Func_8092304(0x14, -0x10, 0) pin is load-bearing and the second is
 * inert -- the first destroys the CSE, the second only re-destroys it.
 *
 * NO NAMED LOCALS AND NOTHING TO STRIP.  `push {lr}` alone proves the ROM
 * holds no value across any call, so there is no local to name and no
 * scaffolding to remove.
 *
 * NO SYMBOL SPELLINGS.  objcmp reports 250 relocations identical and the
 * reference has exactly 250 `bl` instructions, so every relocation is a call
 * target and NO pooled constant carries one on either side: 0x96b, 0x2021,
 * 0x101, 0x105, 0x14ccc, 0xa666, 0xcccc and 0x6666 are bare literals.  The
 * .s writes them as `=0x...` with no `_MSG_` word, unlike the sibling
 * ovl_30_c_a_a_c_c_c_c_b.s which does carry `.word _MSG_2280`.
 *
 * THE SHIFTED-BYTE SPELLING IS COSMETIC.  Rewriting every `0x80 << 9` style
 * constant as its flat literal (0x10000, 0x102, 0x2300000, ...) is
 * byte-identical -- inside a pin the whole value goes to one register and gcc
 * picks the encoding.  The `<<` form is kept because it reads as the ROM's
 * `mov`/`lsl` pair.
 *
 * No Makefile rule or wildcard names rom_7d768c, so the tree default -O2
 * applies; objcmp against the original asm/ path and a scratch copy of the ref
 * agree, which is the wildcard check.  tryc freshness confirmed with
 * -fno-omit-frame-pointer as a positive control (echoed, 748 lines / 745
 * differing -- the probe moves).
 *
 * LANDING NEEDS A SPLIT.  The .s holds TWO functions and this is the SECOND;
 * OvlFunc_952_2008af8 (496 instructions) is the first.
 * overlays/rom_7d768c/overlay.ld:30 names the single .o.  No `.section .data`
 * in the file, so tools/split_s.py's ordinary path applies.
 */
extern void __SetFlag(int f);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int id);
extern void __ActorMessage(int a, int b);
extern void __Func_808e118(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_952_2008ff8(void)
{
    __SetFlag(0x96b);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x2021);
    { PIN3; q0 = 0; q1 = 0x82 << 2; q2 = 0xd4 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80933f8(0x8c << 18, -1, 0xb8 << 17, 1);
    __Func_8093530();
    { PIN3; q0 = 0x14; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x14, 0x28, 0);
    { PIN3; q0 = 0x14; q1 = 0x92 << 2; q2 = 0xb4 << 1;
      __Func_80921c4(q0, q1, q2); }
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0x84 << 2; q2 = 0xb0 << 1;
      __Func_809218c(q0, q1, q2); }
    __Func_80921c4(0x16, 0x84 << 2, 0xb8 << 1);
    __WaitFrames(3);
    __MapActor_SetAnim(0x15, 1);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x14; q1 = -0x10; q2 = 0;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0xa);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x15, 4);
    __MapActor_DoAnim(0x16, 4);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x15; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x15, 8, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x101; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x16, 8, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x15, 3);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x15; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    __Func_8092848(0x15, 0x16, 0x3c);
    __Func_809280c(0x15, 0x14, 0);
    __Func_809280c(0x16, 0x14, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x15; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN2; q0 = 0x14; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x15; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    __Func_8092848(0x15, 0x16, 0x3c);
    __Func_809280c(0x15, 0x14, 0);
    __Func_809280c(0x16, 0x14, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x14; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x15; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0x14);
    __Func_8092848(0x15, 0x16, 0x3c);
    __Func_809280c(0x15, 0x14, 0);
    __Func_809280c(0x16, 0x14, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    { PIN2; q0 = 0x15; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x16; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_809259c(0x15, 2);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_8092adc(0x14, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x14; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x15, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x14; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x28);
    __Func_8092848(0x15, 0x16, 0x3c);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x16, 3);
    __CutsceneWait(0x14);
    __Func_8092adc(0x16, 0x80 << 7, 0);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x15; q1 = 0x14ccc; q2 = 0xa666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x14ccc; q2 = 0xa666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80922c4(0x15, 0, 0x78);
    __Func_8092304(0x16, 0, 0x78);
    __MapActor_SetPos(0x15, 0, 0);
    __MapActor_SetPos(0x16, 0, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x14; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x14, -0x10, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(0x14, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x14, 4);
    __CutsceneWait(0x1e);
    __Func_8092adc(0x14, 0, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x14; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0x14, 0x78, 0);
    __Func_8092304(0x14, 0x3c, 0);
    __MapActor_SetPos(0x14, 0, 0);
    __CutsceneEnd();
}
