extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);

void OvlFunc_883_20091d8(void)
{
    int id;

    if (__GetFlag(0x808) == 0) {
        __CutsceneStart();
        __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
        id = 0xf4d;
        __MessageID(id);
        __Func_8093040(0xf, 0, 2);
        __Func_8093040(0x10, 0, 2);
        __Func_801776c(id + 2, 1);
        __CutsceneWait(6);
        __Func_80921c4(0, 0x45, 0x366);
        __CutsceneEnd();
    }
}
