// fakematch
/* OvlFunc_953_200a964  --  0x0200a964
 * [asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_c_c.s, the only function in the TU]
 *
 * THE TWIN OF OvlFunc_953_200a668, and byte-identical to it in everything but
 * two constants. Diffing the two disassemblies is the whole of the work here:
 * 164 instructions each, and exactly two lines differ -- the message id
 * (0x2134 against 0x2138) and the last call's argument (0x43 against 0x40).
 * Byte-exact: 440 bytes, 170 encodings and 46 relocations identical, matching
 * on the first screen with the sibling's spelling and those two substitutions.
 *
 * The levers are the sibling's, unchanged, and the reasoning is recorded in
 * full at src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_c_b.c. In short:
 *
 *  - NO CONSTANT-CSE PROBLEM. Plain C is already the ROM's length; every
 *    differing line is argument fill order.
 *  - DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER. Five __MapActor_SetSpeed sites
 *    take the same constant pair and the ROM emits three of them one way and
 *    two the other; all five want the SAME uniform source form, and sched2
 *    produces both emitted orders from it. Copying each site's emitted order
 *    instead reverses its movs.
 *  - FOURTEEN PINS, at a greedy fixpoint. Ten of the twenty-two candidate
 *    sites are individually inert, removing all ten is far worse, and two of
 *    the ten become load-bearing once the other eight are gone.
 *
 * WHEN A FUNCTION HAS A TWIN, DIFF THE DISASSEMBLY FIRST. Two constants is a
 * five-minute landing; screening it from scratch would have re-derived
 * fourteen pins and a greedy minimisation for nothing.
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

void OvlFunc_953_200a964(void)
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
    __MessageID(0x2138);
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
    __Func_8091e9c(0x40);
}
