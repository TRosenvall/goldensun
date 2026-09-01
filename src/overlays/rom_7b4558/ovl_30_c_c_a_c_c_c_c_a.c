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
extern void __MapActor_Surprise(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_927_2009880(void)
{
    unsigned char *e;
    int x;
    int y;

    e = __MapActor_GetActor(0xe);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xe, 1);
    OvlFunc_927_2008d90(0xe, 0xc4 << 1, 0xfc << 1, 0xc0 << 11);
    __CutsceneWait(0xa);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 11), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xe, 1);
    __Func_8092848(0xe, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0xe, 2);
    __MapActor_Surprise(0xe, 0x81 << 1);
    x = 0x84 << 2;
    y = 0xc0 << 10;
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0xe, 0xb4 << 1, x, y);
    __Func_809280c(0, 0xe, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xe, 0xa4 << 1, x, y);
    __Func_809280c(0, 0xe, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xe, 0x90 << 1, x, y);
    __Func_809280c(0, 0xe, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xe, 0x80 << 1, x, y);
    __Func_809280c(0, 0xe, 0);
    __CutsceneWait(6);
    __SetCameraTarget(0, 1);
    __MapActor_SetPos(0xe, 0, 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x306);
    __MapActor_SetPos(0x11, 0, 0);
    __CutsceneEnd();
}
