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

void OvlFunc_927_20096f0(void)
{
    unsigned char *e;
    int k;

    e = __MapActor_GetActor(0xd);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xd, 1);
    OvlFunc_927_2008d90(0xd, 0xe4 << 1, 0x68, 0xe0 << 11);
    __CutsceneWait(0xa);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 11), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xd, 1);
    __Func_8092848(0xd, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0xd, 2);
    __MapActor_Surprise(0xd, 0x81 << 1);
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0xd, 0xec << 1, 0x88, 0xc0 << 10);
    __Func_809280c(0, 0xd, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xd, 0xfc << 1, 0x88, 0x33333);
    __Func_809280c(0, 0xd, 0);
    __CutsceneWait(6);
    k = 0xe0 << 10;
    OvlFunc_927_2008d90(0xd, 0x8a << 2, 0x88, k);
    __Func_809280c(0, 0xd, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xd, 0x92 << 2, 0x88, k);
    __Func_809280c(0, 0xd, 0);
    __CutsceneWait(6);
    __MapActor_SetPos(0xd, 0, 0);
    __SetFlag(0xc1 << 2);
    __MapActor_SetPos(0x10, 0, 0);
    __CutsceneEnd();
}
