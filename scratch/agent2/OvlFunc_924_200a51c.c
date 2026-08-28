extern void __CutsceneStart(void);
extern int *__MapActor_GetActor(int a);
extern void __SetFlag(int f);
extern void __ClearFlag(int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneEnd(void);

void OvlFunc_924_200a51c(void)
{
    int *a;
    int i;
    int f;
    int v;
    int t;

    __CutsceneStart();
    f = 0x330;
    for (i = 0; i <= 3; i++) {
        a = __MapActor_GetActor(i + 0xf);
        v = a[2] / 0x100000;
        t = i * 4;
        if (v == t + 0x27) {
            __SetFlag(f);
            __ClearFlag(f + 1);
        } else if (v == t + 0x29) {
            __SetFlag(f + 1);
            __ClearFlag(f);
        } else {
            __ClearFlag(f);
            __ClearFlag(f + 1);
        }
        f += 2;
    }
    a = __MapActor_GetActor(0x13);
    v = a[2] / 0x100000;
    if (v == 0x39) {
        __SetFlag(0x338);
        __ClearFlag(0x339);
        __Func_8010704(0x35, 0xa, 1, 1, 0x3a, 7);
    } else if (v == 0x3b) {
        __SetFlag(0x339);
        __ClearFlag(0x338);
        __Func_8010704(0x35, 0xa, 1, 1, 0x3a, 7);
    } else {
        __ClearFlag(0x338);
        __ClearFlag(0x339);
        __Func_8010704(0x35, 0xb, 1, 1, 0x3a, 7);
    }
    __CutsceneEnd();
}
