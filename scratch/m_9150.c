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

void OvlFunc_927_2009150(void)
{
    unsigned char *e;
    int y;

    e = __MapActor_GetActor(0xa);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xa, 1);
    OvlFunc_927_2008d90(0xa, 0x58, 0x78, 0xc0 << 11);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0xc0 << 13), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xa, 1);
    __Func_8092848(0xa, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0xa, 2);
    __MapActor_Surprise(0xa, 0x81 << 1);
    y = 0xc0 << 10;
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0xa, 0x58, 0x98, y);
    __Func_809280c(0, 0xa, 0);
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0xa, 0x78, 0xc0, y);
    __Func_809280c(0, 0xa, 0);
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0xa, 0x78, 0xf0, y);
    __Func_809280c(0, 0xa, 0);
    __CutsceneWait(0xa);
    __SetFlag(0xc0 << 2);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __CutsceneEnd();
}
