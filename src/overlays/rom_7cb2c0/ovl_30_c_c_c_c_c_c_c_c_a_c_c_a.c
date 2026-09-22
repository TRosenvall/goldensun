/* WHOLE-FILE CONVERSION of
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c_a.s -- OvlFunc_945_200dd10
 * is its only function, so no split and no linker change.  397 instructions, 1024
 * bytes, 411 encodings and 91 relocations identical.
 *
 * Path: 403 of 411 -> 10 -> 4 -> 0.  22 pins, minimal by greedy drop from 29.
 *
 * THE DECISIVE LEVER WAS LIVE-RANGE MERGING, NOT PINS, AND IT WAS 403 -> 10 IN ONE
 * EDIT.  The ROM colours EIGHT pseudos with FIVE registers by merging `a0` with the
 * two later script pointers and the flag variable `v` with the first; gcc chose a
 * six-colouring.  Forcing both merges -- reusing `a0` and `v` as the script holders,
 * with __MapActor_RunScript / __MapActor_SetBehavior declared int-taking so no cast
 * fights it -- removed the extra register.
 *
 * Compare src/rom_b0000/rom_b0070_c_c_a_c_c_a_a.c from the same batch, where the
 * OPPOSITE edit was needed (separating two values the ROM keeps in one register).
 * The two together are the rule: COUNT THE ROM'S REGISTERS AGAINST ITS LIVE VALUES.
 * Fewer registers than values means merge; a shared register with disjoint ranges
 * means leave them separate.
 *
 * THE REMAINING 6 CLOSED BY WRITING ALL EIGHT SetSpeed WINDOWS UNIFORMLY
 * (`q1; q2; q0; q1 <<= 9; q2 <<= 8;`) RATHER THAN TRANSCRIBING THE ROM'S THREE
 * DIFFERENT SHIFT ORDERS.  The ROM's per-site variation is sched2's output, not its
 * input -- transcribing it fights the scheduler.  Last 4 by
 * `q0 = 0xdb << 17; q1 = -1; q2 = 0xae << 16; q3 = 0x1000001;`.
 *
 * No per-file Makefile flag override exists for this stem.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_945__0200e7c8[];
extern unsigned char gScript_945__0200e904[];
extern unsigned char gScript_945__0200e938[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern void __DeleteFieldActor(int slot);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __MapActor_RunScript(int slot, int s);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_945_200b7b4(void);
extern void OvlFunc_945_200c670(int n);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_200c880(int slot, int v);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern int OvlFunc_945_200cfa8(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_945_200dd10(void)
{
    int a0, a1, a2, a3;
    int v;

    a0 = OvlFunc_945_200cfa8(0, 0);
    a1 = OvlFunc_945_200cfa8(1, 0);
    a2 = OvlFunc_945_200cfa8(2, 0);
    a3 = OvlFunc_945_200cfa8(3, 0);
    __CutsceneStart();
    OvlFunc_945_200b7b4();
    OvlFunc_945_200c8e8(0xa, 0, 0);
    OvlFunc_945_200c8e8(0x11, 0, 0);
    { PIN3; q1 = 0xec; q2 = 0x98; q0 = 0x8; q1 <<= 17; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xdc; q2 = 0x86; q0 = 0x1b; q1 <<= 17; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8092950(0x1b, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 0);
    OvlFunc_945_200c670(0x10);
    __MapActor_SetAnim(0x9, 0x5);
    OvlFunc_945_200c8ac(0xdb << 17, -1, 0xae << 16, 0x1000001);
    OvlFunc_945_200c8e8(0x8, 0x1, 0x14);
    __Func_8092950(0x1b, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 1);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x1b; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xcc; q0 = 0x1b; q1 <<= 1; q2 = 0x84;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xcc; q0 = 0x1b; q1 <<= 1; q2 = 0x8e;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 0x1b; q1 <<= 6;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x1b, 0x2);
    __MessageID(0x1f29);
    OvlFunc_945_200c86c(0x1b);
    __CutsceneWait(0x78);
    OvlFunc_945_200c8e8(0xc, a0, 0);
    OvlFunc_945_200c8e8(0xc, a1, 0x1);
    OvlFunc_945_200c8e8(0xc, a2, 0);
    OvlFunc_945_200c8e8(0xc, a3, 0x1);
    OvlFunc_945_200c8e8(0xb, 0, 0);
    { PIN3; q1 = 0xd0; q0 = a0; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = a1; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = a2; q1 <<= 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(a3, 0xb0 << 8, 0x3c);
    v = 0;
    if (__GetFlag(0x934) != 0) {
        v = 2;
    } else if (__GetFlag(0x933) != 0 || __GetFlag(0x92f) != 0) {
        v = 1;
    }
    __Func_80925cc(a0, 0x1);
    if (v == 1)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    else if (v == 2)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    __Func_809259c(a0, 0x2);
    OvlFunc_945_200c86c(a0);
    __MessageID(0x1f2d);
    __MapActor_DoAnim(0x1b, 0x4);
    OvlFunc_945_200c86c(0x1b);
    { PIN3; q1 = 0x81; q0 = a0; q1 <<= 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = a1; q1 <<= 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = a2; q1 <<= 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(a3, 0x81 << 1, 0x3c);
    OvlFunc_945_200c86c(0x1b);
    { PIN3; q1 = 0xcc; q0 = 0x1b; q1 <<= 1; q2 = 0x84;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xde; q0 = 0x1b; q1 <<= 1; q2 = 0x84;
      __Func_80921c4(q0, q1, q2); }
    __DeleteFieldActor(0x1b);
    __CutsceneWait(0x28);
    if (v == 0) {
        if (__GetFlag(0x92c) != 0 || __GetFlag(0x92d) != 0)
            v = 3;
    }
    if (v == 0)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    else if (v == 1)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    else if (v == 2)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 3;
    OvlFunc_945_200c880(a0, 0);
    OvlFunc_945_200c86c(a0);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = a0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    v = (int)gScript_945__0200e904;
    __MapActor_RunScript(a0, v);
    __Func_8092adc(a1, 0xa0 << 7, 0x0);
    __Func_8092adc(a2, 0, 0);
    __Func_8092adc(a3, 0x80 << 8, 0x28);
    __Func_8092adc(a1, 0xd0 << 8, 0x0);
    __Func_8092adc(a2, 0xb0 << 8, 0x0);
    { PIN3; q1 = 0xa0; q0 = a3; q1 <<= 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = a1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = a2; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = a3; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(a2, v);
    __CutsceneWait(0x28);
    a0 = (int)gScript_945__0200e938;
    __MapActor_RunScript(a1, a0);
    __MapActor_SetBehavior(a1, v);
    __MapActor_RunScript(a3, a0);
    __MapActor_RunScript(a3, v);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x2; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x3; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    a0 = (int)gScript_945__0200e7c8;
    __MapActor_SetBehavior(0x1, a0);
    __MapActor_SetBehavior(0x2, a0);
    __MapActor_RunScript(0x3, a0);
    OvlFunc_945_200c8e8(0x17, 0, 0);
    __ClearFlag(0x927);
    __SetFlag(0x8a << 4);
    __ClearFlag(0x12f);
    __CutsceneEnd();
}
