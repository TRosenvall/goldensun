extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __Func_8091eb0(int a, int b);
extern void __SetFlag(int id);

void OvlFunc_927_2009244(void)
{
    unsigned char *e;
    unsigned char *g;
    int hx;

    e = __MapActor_GetActor(0xb);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xb, 0);
    OvlFunc_927_2008d90(0xb, 0xcc << 1, 0xe4 << 1, 0xc0 << 11);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0xc0 << 13), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xb, 1);
    __Func_8092848(0xb, 0, 0);
    __CutsceneWait(0x1e);
    __Func_809259c(0xb, 2);
    __MapActor_Emote(0xb, 0x103, 0);
    __PlaySound(0x93);
    __CutsceneWait(0x3c);
    hx = *(short *)(__MapActor_GetActor(0) + 0xa);
    OvlFunc_927_2008d90(0xb, hx, *(short *)(__MapActor_GetActor(0) + 0x12),
                        0x80 << 11);
    __CutsceneWait(0xa);
    __SetFlag(0x301);
    __MapActor_SetPos(0xe, 0, 0);
    g = gState;
    g[0x22b] = 3;
    __Func_8091eb0(0x35, 0);
    __CutsceneEnd();
}
