struct Actor {
    unsigned char pad00[0x18];
    int f18;
    unsigned char pad1c[0x44 - 0x1c];
    int f44;
    int f48;
    unsigned char pad4c[0x50 - 0x4c];
    unsigned char *f50;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_05;

extern unsigned char gScript_884__0200a8e8[];
extern unsigned char gScript_884__0200a940[];
extern unsigned char gScript_884__0200a998[];
extern unsigned char gScript_884__0200a9f0[];
extern unsigned char gScript_884__0200aa48[];
extern unsigned char gScript_884__0200ab2c[];
extern unsigned char gScript_884__0200ac00[];

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __ActorMessage(int a, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Func_800fe9c(void);
extern void __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091fa8(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_8092a1c(int a, int b, unsigned char *s);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8095214(void);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void OvlFunc_884_200a2c8(int a, int b);
extern void OvlFunc_884_200a2e0(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_884_2009274(void)
{
    int h;
    register int d __asm__("r5");
    register int e __asm__("r6");
    int m;

    __Func_8092950(0x19, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    { PIN3; q2 = 0x14b0000; q1 = 0; q0 = 0x19; __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    { PIN2; q0 = 0x1019; q1 = 0; __ActorMessage(q0, q1); }
    { PIN3; q1 = 0x80; q0 = 0x17; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x18; q1 <<= 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetPos(0x19, 0, 0);
    { PIN3; q1 = 0xa0; q0 = 0x17; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    h = 0xa0 << 7;
    OvlFunc_884_200a2e0(0x18, h, 0x28);
    __Func_80933d4(0xc0 << 9, 0xc0 << 6);
    { PIN4; q0 = 0xb2; q0 <<= 15; q1 = 0xb0; q3 = 1; q1 <<= 16; q2 = 0x1390000;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    { PIN3; q1 = 0xe0; q0 = 0x17; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    OvlFunc_884_200a2e0(0x18, 0xe0 << 7, 0x28);
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0xc8 << 15, 0x90 << 16, 0x14d0000, 1);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x17; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x18; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x69; q2 = 0x149; q0 = 0x17; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_809218c(0x18, 0x7c, 0x149);
    __MapActor_WaitMovement(0x17);
    __MapActor_SetAnim(0x17, 1);
    __Func_8092adc(0x17, h, 0);
    __MapActor_WaitMovement(0x18);
    __MapActor_SetAnim(0x18, 1);
    __Func_8092adc(0x18, h, 0);
    __Func_8092950(0x19, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 1);
    { PIN3; q0 = 0x19; q1 = 0; q2 = 0x14b0000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x19, 0x25, 0x153);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x17, 3);
    d = 0xd0 << 8;
    { PIN2; q1 = 0; q0 = 0x17; __Func_8092c40(q0, q1); }
    { PIN3; q0 = 0x19; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    OvlFunc_884_200a2e0(0, d, 0xa);
    __Func_8091c7c(0, 0);
    e = 0x80 << 8;
    __CutsceneWait(0x28);
    __Func_8092adc(0x17, 0, 0);
    OvlFunc_884_200a2e0(0x18, e, 0x14);
    __Func_80925cc(0x19, 2);
    OvlFunc_884_200a2c8(0x1019, 0xa);
    { PIN3; q2 = (int)gScript_884__0200ac00; q0 = 0; q1 = 0x10019;
      __Func_8092a1c(q0, q1, (unsigned char *)q2); }
    { PIN3; q0 = 0x19; q1 = 0x5d; q2 = 0x169; __Func_80921c4(q0, q1, q2); }
    OvlFunc_884_200a2e0(0x19, d, 0x28);
    OvlFunc_884_200a2c8(0x19, 0x14);
    __MapActor_SetIdle(0);
    __Func_8092adc(0x17, 0, 0);
    OvlFunc_884_200a2e0(0x18, e, 0xf);
    __MapActor_SetAnim(0x17, 3);
    __MapActor_DoAnim(0x18, 3);
    __Func_8092adc(0x17, h, 0);
    OvlFunc_884_200a2e0(0x18, h, 0x1e);
    __MapActor_DoAnim(0x18, 4);
    __ActorMessage(0x2018, 0);
    OvlFunc_884_200a2e0(0, d, 0x1e);
    OvlFunc_884_200a2e0(0, 0x80 << 7, 0x28);
    __Func_80925cc(0x17, 2);
    __MapActor_DoAnim(0x17, 3);
    OvlFunc_884_200a2c8(0x17, 0x14);
    { PIN2; q1 = 0x81; q0 = 0; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x19, 0x81 << 1);
    __CutsceneWait(0x50);
    __MapActor_SetBehavior(0x18, gScript_884__0200a8e8);
    __CutsceneWait(6);
    __MapActor_SetBehavior(0x17, gScript_884__0200a940);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0, gScript_884__0200a998);
    __CutsceneWait(6);
    __MapActor_RunScript(0x19, gScript_884__0200a9f0);
    gState[0x22b] = 2;
    do { } while (0);
    m = (int)(&_AREA_05);
    __Func_8091f90(m, 0x13);
    __Func_8091fa8(m, 0x13);
    __Func_8091eb0(0xc, 4);
    __SetFlag(0x8d << 1);
}

void OvlFunc_884_20095b4(void)
{
    struct Actor *a;
    register int c1 __asm__("r6");
    register int s __asm__("r5");
    register int k1 __asm__("r3");

    __PlaySound(0x11);
    __CutsceneStart();
    { PIN3; q0 = -1; q1 = -1; q2 = -1;
      __Func_80933f8(q0, q1, q2, 0); }
    __WaitFrames(1);
    __Func_80933f8(0x80 << 15, 0x90 << 16, 0xaf << 17, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    { PIN3; q0 = 0; q1 = 0xc0 << 14; q2 = 0xad << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x19; q1 = 0x9c << 15; q2 = 0xb3 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x17; q1 = 0xce << 15; q2 = 0xab << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x18; q1 = 0xe0 << 15; q2 = 0xb4 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN2; q0 = 0x17; q1 = 0x80 << 8;
      __Func_8092adc(q0, q1, 0); }
    { PIN2; q0 = 0x18; q1 = 0x80 << 8;
      __Func_8092adc(q0, q1, 0); }
    __MapActor_SetAnim(0, 0x10);
    __MapActor_GetActor(0)->f18 = 0xffff0000;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapActor_SetAnim(0x19, 7);
    a = __MapActor_GetActor(0x19);
    k1 = 0x1555;
    *(short *)(a->f50 + 0x1e) = k1;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __MapTransitionIn();
    __Func_8095268();
    __WaitMapTransition();
    __CutsceneWait(0x50);
    { PIN2; q0 = 0x17; q1 = 0xc0 << 6;
      __Func_8092adc(q0, q1, 0); }
    { PIN2; q0 = 0x18; q1 = 0xc0 << 8;
      __Func_8092adc(q0, q1, 0x28); }
    __MapActor_DoAnim(0x17, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x18, 3);
    { PIN2; q0 = 0x17; q1 = 0x80 << 8;
      __Func_8092adc(q0, q1, 0xa); }
    { PIN2; q0 = 0x18; q1 = 0x80 << 8;
      __Func_8092adc(q0, q1, 0xa); }
    __Func_8092b08(0, 3);
    __Func_8092b08(0x19, 3);
    { PIN3; q2 = 0x13333; q1 = 0x26666; q0 = 0x17;
      __MapActor_SetSpeed(q0, q1, q2); }
    a = __MapActor_GetActor(0x17);
    c1 = 0x28f;
    s = 0x80 << 8;
    a->f44 = c1;
    a->f48 = s;
    __MapActor_SetBehavior(0x17, gScript_884__0200aa48);
    __CutsceneWait(0x18);
    { PIN3; q2 = 0x13333; q1 = 0x26666; q0 = 0x18;
      __MapActor_SetSpeed(q0, q1, q2); }
    a = __MapActor_GetActor(0x18);
    a->f44 = c1;
    a->f48 = s;
    __MapActor_RunScript(0x18, gScript_884__0200ab2c);
    __CutsceneWait(0x28);
    __Func_8095214();
    __Func_8095240();
    __WaitFrames(0x14);
    __Func_8095240();
    __WaitFrames(0x3c);
    __Func_8095240();
    __WaitFrames(0x14);
    __Func_8095214();
    __CutsceneWait(0x28);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x78;
    __MapTransitionOut();
    __WaitMapTransition();
    __ClearFlag(0x834);
    __Func_8091e9c(9);
    __CutsceneEnd();
}
