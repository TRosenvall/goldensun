struct A {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[4];
    int f24;
    int f28;
    int f2c;
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[4];
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
};

extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern int __Random(void);
extern struct A *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Actor_SetScript(struct A *a, const void *s);
extern void OvlFunc_883_200d8f0(struct A *a, int b, int c);
extern const unsigned char gScript_883__0200e6e4[];

void OvlFunc_883_200d7fc(struct A *src)
{
    int i;
    struct A *a;
    int r;

    __PlaySound(0x9a);
    for (i = 30; i >= 0; i--) {
        src->fc += 0x10000;
        src->f6 += 0x2000;
        src->f18 += -0x800;
        src->f1c += -0x800;
        __WaitFrames(1);
    }
    for (i = 7; i >= 0; i--) {
        a = __CreateActor(0x11d, src->f8, src->fc, src->f10);
        if (a != 0) {
            __Actor_SetSpriteFlags(a, 0);
            __Actor_SetScript(a, gScript_883__0200e6e4);
            a->f30 = __Random() + 0x10000;
            a->f34 = 0x10000;
            a->f55 = 2;
            a->f48 = 0xa3d;
            r = __Random();
            a->f28 = r - __Random();
            OvlFunc_883_200d8f0(a, __Random() * 24 + 0x80000, __Random());
        }
    }
    __PlaySound(0x83);
    src->f8 = 0;
    src->fc = 0;
    src->f10 = 0;
    src->f38 = 0x80000000;
    src->f3c = 0x80000000;
    src->f40 = 0x80000000;
    src->f24 = 0;
    src->f28 = 0;
    src->f2c = 0;
}
