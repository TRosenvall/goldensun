extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_887_2008f90(void)
{
    unsigned char *p;
    int z;
    unsigned char bit;
    register int m __asm__("r5");

    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x239; q2 = 0x189;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0; q1 <<= 7;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(8, 2);
    __MessageID(0x1c66);
    __Func_8093040(8, 0, 0x50);
    { PIN3; q2 = 0x3c; q0 = 8; q1 = 0x101;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(8, 1);
    __Func_8093040(8, 0, 0x3c);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x50);
    { PIN3; q0 = 8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_TravelTo(8, 0x92 << 2, 0xcb << 1);
    __Func_80118c0(0xb);
    __Func_80118a8(0xc);
    __MapActor_SetAnim(8, 0xc);
    __CutsceneWait(0x50);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x28);
    __Func_8093040(8, 0, 0x28);
    { PIN3; q1 = 0x84; q0 = 8; q1 <<= 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(8, 0, 0x28);
    { PIN3; q2 = 0x3c; q0 = 0; q1 = 0x105;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(8, 0xd);
    { PIN3; q2 = 0; q0 = 8; q1 = 0x103;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetAnim(8, 0xb);
    __CutsceneWait(0x28);
    __Func_8093040(8, 0, 0x28);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(8, 0xc);
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 0; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetAnim(8, 0xd);
    { PIN2; q1 = 0; q0 = 8;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN1; q0 = 0x81c;
      if (__GetFlag(q0) != 0) {
        PIN3; q1 = 0x81; q0 = 8; q1 <<= 1; q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
      } }
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    { PIN3; q1 = 0x107; q2 = 0x3c; q0 = 8;
      __MapActor_Emote(q0, q1, q2); }
    do { } while (0);
    m = 0x1c6f;
    __MessageID(m);
    { PIN2; q1 = 0; q0 = 8;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    if (__GetFlag(0x81c) != 0) {
        PIN3; q1 = 0x81; q0 = 8; q1 <<= 1; q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __MapActor_Emote(8, 0x107, 0x3c);
    __MessageID(m + 3);
    __ActorMessage(8, 0);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 0xd);
    __Func_809259c(8, 2);
    __Func_8093040(8, 0, 0x28);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x28);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x28);
    p = __MapActor_GetActor(0);
    z = 0;
    *(short *)(p + 6) = z;
    __WaitFrames(1);
    p = __MapActor_GetActor(0) + 0x5a;
    *p = 0xfe & *p;
    { PIN3; q1 = 0x22e; q0 = 0; q2 = 0xc2; q2 <<= 1;
      __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q2 = 0x9999; q0 = 8; q1 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(8, 0xe);
    __Func_8092158(8, 0x24a, 0xc8 << 1);
    __CutsceneWait(0x28);
    { PIN3; q1 = 0x91; q2 = 0xbf; q0 = 8; q1 <<= 2; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(8, 0x80 << 8, 0x28);
    p = __MapActor_GetActor(0) + 0x5a;
    bit = 1;
    *p = bit | *p;
    __Func_8092adc(8, 0xc0 << 8, 8);
    __Func_8092adc(8, 0, 8);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 7; q2 = 8;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Jump(8, 4, 0x14);
    __MapActor_Jump(8, 6, 0x28);
    __MapActor_Jump(8, 4, 0x14);
    __Func_8093040(8, 0, 0x28);
    { PIN3; q0 = 8; q1 = 0x6666; q2 = 0x3333;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x8f; q2 = 0xc0; q0 = 8; q1 <<= 2; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __SetFlag(0x81e);
    __SetFlag(0x203);
    __CutsceneEnd();
}
