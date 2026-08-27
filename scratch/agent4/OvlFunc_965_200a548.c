extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_965_200a548(void)
{
    int x;
    int m;
    int n;

    x = 0x94 << 1;
    if (__GetFlag(0x985) != 0) {
        __ClearFlag(0x985);
        __PlaySound(0x9d);
        __CutsceneStart();
        __MapActor_TravelTo(8, x, 0xf0);
        __MapActor_TravelTo(9, 0x9c << 1, 0xf0);
        __MapActor_WaitMovement(8);
        __MapActor_WaitMovement(9);
        m = 0x11;
        n = 0xe;
        __Func_8010704(0, 0xe, 4, 1, m, n);
        __CutsceneEnd();
        if (__GetFlag(0x301) != 0)
            __ClearFlag(0x301);
        else
            __SetFlag(0x301);
    }
}
