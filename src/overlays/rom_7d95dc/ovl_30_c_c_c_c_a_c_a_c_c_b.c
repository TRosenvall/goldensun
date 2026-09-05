// fakematch
/* OvlFunc_953_200a668  --  0x0200a668
 * [asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_c.s, first of two functions]
 *
 * 164 instructions of straight-line cutscene: place three actors, transition
 * in, run a conversation, hand three of them a shared behaviour script, then
 * walk two of them out. Byte-exact: 440 bytes, 170 encodings and 46
 * relocations identical.
 *
 * NO CONSTANT-CSE PROBLEM HERE, which is worth saying because the shape looks
 * like one. Plain C is already 164 lines against 164 -- the register count is
 * right from the start, and `push {r5, lr}` matches, r5 holding the script
 * pointer in both. All 147 differing lines were ARGUMENT FILL ORDER and
 * nothing else. Check the length before reaching for the CSE cures.
 *
 * DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER. This function is the clearest
 * demonstration of the recorded rule that the movs are slaved to the SOURCE
 * shift order and sched2 re-lands the shifts itself.
 *
 * Five __MapActor_SetSpeed sites take the same (0x80 << 9, 0x80 << 8) pair.
 * The ROM emits three of them as `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`
 * and the other two as `mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1` -- the
 * shift and the slot mov swap places. Written to match each site's EMITTED
 * order, the two odd ones come out with their MOVS reversed instead. Written
 * with the SAME uniform source form as their three siblings --
 * `q1; q2; q0; q1 <<= 9; q2 <<= 8;` -- all five are exact, and sched2 produces
 * both emitted orders from the one spelling.
 *
 * So a site that looks different from its siblings in the ROM is not
 * necessarily different in the source. Three sites (a SetPos, a TravelTo and
 * the second SetSpeed pair) cost 10 differing between them until the shifts
 * were written ascending instead of copied.
 *
 * FOURTEEN PINS, MINIMISED TO A GREEDY FIXPOINT. All 22 candidate sites were
 * stripped one at a time: TEN were individually inert. Removing all ten
 * together is 169 lines and 160 differing -- badly worse -- so a greedy pass
 * was run instead, dropping one at a time and keeping the drop only if the
 * result was still exact. Eight came out; the last two BREAK once the other
 * eight are gone, having been inert only while those were present.
 *
 * That is the third measurement of "N pins is a size, not a set", and the
 * sharpest form of it: an individually-inert pin can become load-bearing when
 * other inert pins are removed, so the one-at-a-time list is a set of
 * CANDIDATES, never a set of removals.
 *
 * No wildcard captures this object; tree default -O2.
 */
extern unsigned char gScript_953__0200adac[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MessageID(int id);
extern void __Func_8091e9c(int a);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_953_2009c48(int a);
extern void OvlFunc_953_2009c5c(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_953_200a668(void)
{
    __CutsceneStart();
    { PIN3; q1 = 0xc6; q2 = 0x88; q0 = 1; q1 <<= 18; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xce; q2 = 0x88; q0 = 2; q1 <<= 18; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(3, 0xca << 18, 0x98 << 16);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(8, 1);
    __MapActor_SetAnim(8, 3);
    __MessageID(0x2134);
    OvlFunc_953_2009c48(8);
    __Func_80925cc(9, 1);
    OvlFunc_953_2009c48(9);
    __Func_80925cc(0xa, 1);
    OvlFunc_953_2009c48(0xa);
    __Func_80925cc(0xb, 1);
    __MapActor_SetAnim(0xb, 3);
    OvlFunc_953_2009c48(0xb);
    { PIN3; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 2; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 3; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(1, gScript_953__0200adac);
    __MapActor_SetBehavior(2, gScript_953__0200adac);
    __MapActor_RunScript(3, gScript_953__0200adac);
    __CutsceneWait(0x14);
    OvlFunc_953_2009c5c(0, 0);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(0xb, 3);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xb; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 2);
    { PIN3; q0 = 0xb; q1 = 0x33e; q2 = 0x98; __Func_8092158(q0, q1, q2); }
    { PIN3; q1 = 0xca; q0 = 0xb; q1 <<= 2; q2 = 0xa4; __Func_8092158(q0, q1, q2); }
    __MapActor_TravelTo(0xb, 0xca << 2, 0x9c << 1);
    __CutsceneWait(0x14);
    __Func_80933d4(0x6666, 0xccc);
    { PIN4; q0 = 0xca; q1 = 1; q2 = 0x9c; q3 = 1; q0 <<= 18; q1 = -q1; q2 <<= 17;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q1 = 0xca; q0 = 0; q1 <<= 2; q2 = 0xa4; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xca; q2 = 0x9c; q1 <<= 2; q2 <<= 1; q0 = 0;
      __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x43);
}
