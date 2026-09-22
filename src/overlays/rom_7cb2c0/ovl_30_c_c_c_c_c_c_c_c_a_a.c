/* WHOLE-FILE CONVERSION of asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_a.s --
 * OvlFunc_945_200d7ec is its only function and the .s carries no .data/.bss, so no
 * split and no linker change.  428 instructions, 1116 bytes, 437 encodings and 115
 * relocations identical.
 *
 * Path: 389 of 437 -> 10 (pins) -> 0.  THIRTY-THREE PINS, and that count is
 * MINIMAL -- a two-round greedy drop from 40 found 7 droppable and each of the
 * remaining 33 breaks it.  This is the batch-280 rule in practice: on a long
 * straight-line cutscene script the pin count scales with the script's repetition,
 * not with how well the function was read.
 *
 * Three sequential `int` locals replay the ROM's single reuse of r5 (-1,
 * 0xc0 << 7, 0x80 << 8) plus the script pointer.
 *
 * THE LAST 10 WERE PURE ARGUMENT-WINDOW ORDERING and needed two different
 * spellings at two different callees -- `q1; q2; q0; q1 <<= 17; q2 <<= 16;` for
 * __MapActor_SetPos, and INLINE SHIFTS `q0 = 0x80 << 10; q1 = 0x80 << 10;
 * q2 = 0x80 << 9;` for __Func_8012330.  Separated-shift and inline-shift forms are
 * not interchangeable and the choice is per callee.
 *
 * No per-file Makefile flag override exists for this stem (verified by grep, not by
 * relying on tryc's detector).
 */
extern int L7f84 __asm__(".L7f84");
extern unsigned char gScript_945__0200e818[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __ActorMessage(int a, int b);
extern void __DeleteFieldActor(int slot);
extern int __StartTask(void (*fn)(void), int n);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_945_200c670(int n);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_200c880(int slot, int v);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200dc48(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_945_200d7ec(void)
{
    int a0, a1, a2;
    int m1, h, s;
    unsigned char *sc;

    a0 = OvlFunc_945_200cfa8(0, 0);
    a1 = OvlFunc_945_200cfa8(1, 0);
    a2 = OvlFunc_945_200cfa8(2, 0);
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xa, 0, 0);
    OvlFunc_945_200c8e8(0x11, 0, 0);
    { PIN3; q1 = 0xec; q2 = 0x98; q0 = 0x8; q1 <<= 17; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(0x9, 0x5);
    { PIN3; q1 = 0xdc; q2 = 0x86; q0 = 0x1b; q1 <<= 17; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8092950(0x1b, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 0);
    OvlFunc_945_200c670(0x10);
    m1 = -1;
    OvlFunc_945_200c8ac(0xdb << 17, m1, 0xae << 16, 0x1000001);
    OvlFunc_945_200c8e8(0x8, 0x1, 0x14);
    __PlaySound(0x13);
    __PlaySound(0xb5);
    { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q1 = m1; q2 = 0xe666; q0 = m1;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x50);
    __PlaySound(0xb5);
    { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8012330(m1, m1, 0xe666);
    __PlaySound(0x3f);
    __SetFlag(0x8d << 1);
    __MapActor_Surprise(0x3, 0x81 << 1);
    h = 0xc0 << 7;
    __CutsceneWait(0x28);
    OvlFunc_945_200c880(0x3, h);
    __MessageID(0x1ec1);
    __Func_8093040(0x3, 0, 0x28);
    OvlFunc_945_200c86c(0x1b);
    { PIN3; q1 = 0x80; q0 = 0x0; q1 <<= 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0x1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x2, 0, 0);
    { PIN3; q1 = 0xe0; q0 = 0x3; q1 <<= 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x0, h, 0);
    { PIN3; q1 = 0xe0; q0 = 0x1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x2, h, 0);
    { PIN3; q1 = 0x80; q0 = 0x3; q1 <<= 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xe0; q0 = 0x2; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x3c; q0 = 0x2; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_945_200c880(0x2, 0x80 << 6);
    __Func_80925cc(0x2, 0x1);
    OvlFunc_945_200c86c(0x2);
    __Func_809259c(0x0, 0x1);
    __Func_809259c(0x1, 0x1);
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0xa);
    { PIN3; q1 = 0xc0; q0 = 0x0; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xe0; q0 = 0x2; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q2 = 0x14; q0 = 0x3; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092950(0x1b, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 1);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x1b; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xd7; q2 = 0x86; q0 = 0x1b; q1 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    OvlFunc_945_200c880(0x1b, 0xc0 << 6);
    __Func_809259c(0x1b, 0x2);
    OvlFunc_945_200c86c(0x1b);
    __Func_809259c(a0, 0x1);
    __Func_809259c(a1, 0x1);
    __Func_809259c(a2, 0x1);
    __Func_80925cc(0xd, 0x1);
    { PIN2; q1 = 0x81; q0 = a0; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q0 = a1; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q0 = a2; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0xd;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    OvlFunc_945_200c8e8(0xc, a0, 0);
    OvlFunc_945_200c8e8(0xc, a1, 0x1);
    OvlFunc_945_200c8e8(0xc, a2, 0);
    OvlFunc_945_200c8e8(0xb, 0x1, 0);
    __Func_8092adc(a0, 0xd0 << 8, 0x0);
    __Func_8092adc(a1, 0xb0 << 8, 0x0);
    { PIN3; q1 = 0xd0; q0 = a2; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x0, 0, 0);
    { PIN3; q1 = 0x80; q0 = 0x1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x2, 0, 0);
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0x3; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x1b, 0x2);
    __ActorMessage(0x1b, 0);
    { PIN3; q1 = 0xc0; q0 = 0x0; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x2; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x3; q1 <<= 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xdc; q0 = 0x1b; q1 <<= 1; q2 = 0x86;
      __Func_80921c4(q0, q1, q2); }
    s = 0x80 << 8;
    __MapActor_SetPos(0x1b, 0, 0);
    OvlFunc_945_200c880(0x1, s);
    __Func_80925cc(0x1, 0x1);
    OvlFunc_945_200c86c(0x1);
    __Func_8092adc(0x2, 0, 0);
    OvlFunc_945_200c880(0x3, s);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    { PIN3; q1 = 0x80; q2 = s; q0 = 0x1; q1 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = s; q0 = 0x2; q1 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = s; q0 = 0x3; q1 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    sc = gScript_945__0200e818;
    __MapActor_SetBehavior(0x1, sc);
    __MapActor_SetBehavior(0x2, sc);
    __MapActor_RunScript(0x3, sc);
    __SetFlag(0x302);
    L7f84 = 0;
    __StartTask(OvlFunc_945_200dc48, 0xc8 << 4);
    OvlFunc_945_200c8e8(0x17, 0, 0);
    __DeleteFieldActor(0x1b);
    __ClearFlag(0x12f);
    __ClearFlag(0x927);
    __CutsceneEnd();
}
