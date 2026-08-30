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

void OvlFunc_927_2009328(void)
{
    unsigned char *e;
    int x;
    int y;

    e = __MapActor_GetActor(0xc);
    x = 0xac << 1;
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xc, 1);
    OvlFunc_927_2008d90(0xc, 0x86 << 2, x, 0xe0 << 11);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 13), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xc, 1);
    __Func_8092848(0xc, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0xc, 2);
    __MapActor_Surprise(0xc, 0x81 << 1);
    y = 0xc0 << 10;
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0xc, 0x92 << 2, x, y);
    __Func_809280c(0, 0xc, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xc, 0x9e << 2, x, y);
    __Func_809280c(0, 0xc, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xc, 0xaa << 2, x, y);
    __Func_809280c(0, 0xc, 0);
    __CutsceneWait(6);
    __SetFlag(0x302);
    __MapActor_SetPos(0xf, 0, 0);
    __CutsceneEnd();
}
