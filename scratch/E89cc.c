extern int *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_907_20089cc(void)
{
    int *a;
    int *b;
    int u, v, w, t;
    int s;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(0x14);
    u = b[4] >> 20;
    v = a[2] >> 20;
    w = a[4] >> 20;
    t = b[2];
    s = 0xf;
    __Func_8010704(0xf, 0xb, 3, 1, s, 0xc);
    __Func_8010704(0xf, 0xb, 3, 1, s, 0xd);
    __Func_8010704(0xf, 0xb, 3, 1, s, 0xe);
    t >>= 20;
    __Func_8010704(1, 0, 1, 1, t, u);
    if (t != 0x10 || u != 0xd)
        __Func_8010704(0, 0, 1, 1, 0x10, 0xd);
    if (v == 0x10 && w == 0xd) {
        __CutsceneStart();
        __MapActor_Emote(0, 0x80 << 1, 0x14);
        __MapActor_SetSpeed(0, 0x80 << 10, 0x80 << 9);
        __MapActor_Jump(0, 6, 0);
        if (u == 0xd) {
            __Func_8092158(0, 0x83 << 1, 0xc4);
            __Func_8092adc(0, 0x80 << 7, 0x14);
        } else {
            __Func_8092158(0, 0x8f << 1, 0xda);
            __Func_8092adc(0, 0x80 << 8, 0x14);
        }
        __CutsceneEnd();
    }
}
