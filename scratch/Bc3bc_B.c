struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[8];
    short f18;
    unsigned char pad1a[0xe];
};

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern int *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(int *actor, int f);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct P *p);

void OvlFunc_896_200c3bc(void)
{
    struct P p;
    int *a;
    unsigned int i;
    int t;
    int x, y;

    a = __MapActor_GetActor(0xe);
    __PlaySound(0xbe);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8e << 1;
    p.f8 = 0x6666;
    p.fc = 0xc0 << 10;
    i = 0;
    do {
        __CutsceneWait(1);
        t = 1 & i;
        if (t == 0) {
            x = a[2] + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a[3] + ((__Random() << 5) >> 16 << 16);
            y += 0x80 << 14;
            OvlFunc_common0_10c(x, y, a[4], 0, 0xfffc0000, t, 0xd8 << 13, &p);
        }
        if (i == 0x14)
            __Func_8092950(0xe, 0x80 << 1);
        i++;
    } while (i <= 0x1f);
    __Func_8092950(0xe, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 1);
}
