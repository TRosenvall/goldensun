extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_891_2008098(void);

void OvlFunc_891_200a2f4(void)
{
    int c0, c1, c2;
    int e1, f1, e2, f2;

    c0 = 0x8f << 17;
    c1 = -1;
    c2 = 0x92 << 16;
    __CutsceneStart();
    if (__GetFlag(0x818) == 0 && __GetFlag(0x817) == 0) {
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        __Func_80933f8(c0, c1, c2, 1);
        __Func_8093530();
        __PlaySound(0xba);
        e1 = 4;
        f1 = 3;
        __CopyMapTiles(4, 0x3b, 0x11, 0x26, e1, f1);
        if (__GetFlag(0x816) != 0) {
            e2 = 2;
            f2 = 2;
            __CopyMapTiles(8, 0x3c, 0x11, 0x27, e2, f2);
        }
        __Func_8092adc(0, 0x80 << 8, 0);
        __CutsceneWait(0x1e);
        __SetFlag(0x817);
        if (__GetFlag(0x816) != 0)
            OvlFunc_891_2008098();
    }
    __CutsceneEnd();
}
