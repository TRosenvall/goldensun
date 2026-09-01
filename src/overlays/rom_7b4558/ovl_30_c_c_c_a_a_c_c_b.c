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

void OvlFunc_927_200a078(void)
{
    unsigned char *e;
    int x;
    int w;

    x = 0xb2 << 2;
    e = __MapActor_GetActor(0x12);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0x12, 1);
    OvlFunc_927_2008d90(0x12, x, 0x86 << 2, 0xc0 << 11);
    __CutsceneWait(0xa);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 11), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0x12, 1);
    __Func_8092848(0x12, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0x12, 2);
    __MapActor_Surprise(0x12, 0x81 << 1);
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0x12, x, 0x8e << 2, 0xc0 << 11);
    __Func_809280c(0, 0x12, 0);
    w = 0xc0 << 10;
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0x12, x, 0x96 << 2, w);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    x += 0x18;
    OvlFunc_927_2008d90(0x12, x, 0xa0 << 2, w);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0x12, x, 0xb0 << 2, w);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    __SetCameraTarget(0, 1);
    __MapActor_SetPos(0x12, 0, 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x30b);
    __CutsceneEnd();
}
