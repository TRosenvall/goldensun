extern unsigned char iwram_3001ebc[];
extern unsigned char L20ac[] __asm__(".L20ac");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8091220(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_885_2008964(void)
{
    unsigned char *p;
    int w;

    __CutsceneStart();
    __MapActor_SetPos(0xd, 0, 0);
    { PIN3; q1 = 0xd8; q2 = 0x84; q0 = 1; q1 <<= 16; q2 <<= 17;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(5, 0xf8 << 16, 0x84 << 17);
    w = 0xc0 << 8;
    *(unsigned short *)(__MapActor_GetActor(1) + 6) = w;
    *(unsigned short *)(__MapActor_GetActor(5) + 6) = w;
    __Func_8010560(L20ac, 0x2b, 8);
    *(unsigned int *)(*(unsigned char **)iwram_3001ebc + 0x1c0) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xd; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xe6; q2 = 0xdc; q0 = 0xd; q1 <<= 16; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80921c4(0xd, 0xe6, 0xe8);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xd, 3);
    __MessageID(0xfcc);
    __Func_8093040(0xd, 0, 0xa);
    __Func_80925cc(0xd, 2);
    { PIN3; q1 = 0xc0; q0 = 0xd; q1 <<= 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xd, 0, 0xa);
    __Func_8092adc(1, 0, 0);
    { PIN3; q1 = 0x80; q2 = 0xa; q0 = 0; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __MapActor_SetAnim(0, 0);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0, 0);
    { PIN3; q1 = 0x80; q2 = 0xa; q0 = 5; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(5, 3);
    __MapActor_DoAnim(0, 3);
    __MapActor_SetAnim(0, 0);
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092adc(0, w, 0);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(5, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(5, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(0xd, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0xd, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(5, 0, 0);
    __MapActor_WaitMovement(0xd);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(5, 1);
    __MapActor_SetAnim(0xd, 1);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __SetFlag(0x801);
    *(unsigned int *)(*(unsigned char **)iwram_3001ebc + 0x1c0) = 0x100;
    __Func_8091220(0x80 << 9, 0);
    __SetFlag(0x242);
    __CutsceneEnd();
}
