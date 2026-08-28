extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_801776c(int a, int b);
extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_809202c(void);

void OvlFunc_932_2008bd8(void)
{
    int a;
    int b;
    int c;
    int d;

    __CutsceneStart();
    __Func_801776c(0x1528, 1);
    __PlaySound(0x7d);
    if (__GetFlag(0x327) != 0) {
        a = 0x1d;
        b = 0x51;
        __Func_8010704(0x1c, 0x52, 1, 1, a, b);
        c = 1;
        d = 2;
        __CopyMapTiles(0x2f, 0x1c, 0x1d, 0x11, c, d);
        __ClearFlag(0x327);
    } else {
        a = 0x1d;
        b = 0x51;
        __Func_8010704(0x1e, 0x52, 1, 1, a, b);
        c = 1;
        d = 2;
        __CopyMapTiles(0x2e, 0x1c, 0x1d, 0x11, c, d);
        __SetFlag(0x327);
    }
    __WaitFrames(0x14);
    __Func_809202c();
    __CutsceneEnd();
}
