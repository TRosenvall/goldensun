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
    int p1, p2, q1, q2, r1, r2;

    __CutsceneStart();
    i = 0;
    f = 0x330;
    for (; i <= 3; i++) {
        v = __MapActor_GetActor(i + 0xf)[2] / 0x100000;
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
    v = __MapActor_GetActor(0x13)[2] / 0x100000;
    if (v == 0x39) {
        __SetFlag(0x338);
        __ClearFlag(0x339);
        p1 = 0x3a;
        p2 = 7;
        __Func_8010704(0x35, 0xa, 1, 1, p1, p2);
    } else if (v == 0x3b) {
        __SetFlag(0x339);
        __ClearFlag(0x338);
        q1 = 0x3a;
        q2 = 7;
        __Func_8010704(0x35, 0xa, 1, 1, q1, q2);
    } else {
        __ClearFlag(0x338);
        __ClearFlag(0x339);
        r1 = 0x3a;
        r2 = 7;
        __Func_8010704(0x35, 0xb, 1, 1, r1, r2);
    }
    __CutsceneEnd();
}
