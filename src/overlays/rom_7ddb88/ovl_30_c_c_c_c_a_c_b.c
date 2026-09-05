extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_955_20090dc(int a)
{
    unsigned char *p;
    int x, y, t;

    p = __MapActor_GetActor(a);
    x = *(short *)(p + 0xa);
    y = *(short *)(p + 0x12);
    __CutsceneStart();
    { PIN3; q0 = a; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetPos(0, x << 16, (y << 16) - 0x300000);
    __MapActor_SetPos(1, (x << 16) - 0x100000, (y << 16) - 0x280000);
    __MapActor_SetPos(2, (x << 16) + 0x100000, (y << 16) - 0x280000);
    __MapActor_SetPos(3, x << 16, (y << 16) - 0x200000);
    __MapActor_SetPos(a, x << 16, (y << 16) - 0x500000);
    t = 0xc0 << 8;
    *(short *)(__MapActor_GetActor(0) + 6) = t;
    __SetCameraTarget(0, 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __MessageID(0x20e9);
    __MapActor_DoAnim(a, 3);
    __ActorMessage(a, 0);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    __MapActor_SetAnim(3, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(6);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __Func_80921c4(a, x - 0x10, y - 0x40);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetPos(3, 0, 0);
    __Func_80921c4(a, x - 0x10, y - 0x10);
    __Func_80921c4(a, x, y);
    __Func_8092adc(a, t, 0xa);
    __CutsceneEnd();
}
