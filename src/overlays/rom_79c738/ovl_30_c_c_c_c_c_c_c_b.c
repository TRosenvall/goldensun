extern int __GetFlag(int flag);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_80921c4(int a, int b, int c);

void OvlFunc_909_2008568(void)
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;

    a = 0x80 << 1;
    b = 0xe0 << 7;
    c = 0x80 << 9;
    d = 0x80 << 8;
    e = 0x9a << 2;
    f = 0xd0 << 8;
    g = 0x2fa;
    if (__GetFlag(0x84e) != 0)
        return;
    if (__GetFlag(0x322) == 0)
        return;
    __CutsceneStart();
    __MapActor_Emote(0x13, a, 0);
    __Func_8092adc(0x13, b, 0xa);
    __Func_80925cc(0x13, 2);
    __CutsceneWait(0x14);
    __MessageID(0x1748);
    __ActorMessage(0x13, 0);
    __MapActor_SetSpeed(0, c, d);
    __Func_80921c4(0, e, g);
    __Func_8092adc(0x13, f, 0xa);
    __CutsceneEnd();
}
