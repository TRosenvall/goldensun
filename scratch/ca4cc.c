struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x3d];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_947_2008528(int a, int b, int c, int d, int e, int f);

void OvlFunc_947_200a4cc(void)
{
    struct A *a;
    int f;

    a = __MapActor_GetActor(0xa);
    __CutsceneStart();
    OvlFunc_947_2008528(2, a->f8 >> 20, a->f10 >> 20, 1, 1, 0xff);
    if ((a->f8 >> 20) == 0x10) {
        f = __GetFlag(0x81 << 2);
        if (f == 0) {
            __CutsceneWait(0xa);
            __PlaySound(0x9f);
            a->f55 = f;
            a->f14 = 0xfffe0000;
            a->fc = 0xfffe0000;
            __SetFlag(0x81 << 2);
        }
    }
    __CutsceneEnd();
}
