extern void __CutsceneStart(void);
extern int *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __PlaySound(int id);
extern void __SetFlag(int flag);
extern void __CutsceneEnd(void);

void OvlFunc_958_2009158(void)
{
    int *a;
    int m;
    int n;
    int y;
    int z;

    y = 0xae << 18;
    z = 0x90 << 17;
    __CutsceneStart();
    a = __MapActor_GetActor(9);
    if ((a[2] >> 20) > 0x2a) {
        m = 0x6b;
        n = 0x11;
        __Func_8010704(0x6c, 0x11, 1, 1, m, n);
        __CutsceneWait(8);
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xa, y, z);
        __MapActor_SetAnim(0xa, 3);
        __PlaySound(0x9a);
        __SetFlag(0x9a5);
    }
    __CutsceneEnd();
}
