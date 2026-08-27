struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x3c - 0x14];
    int f3c;
};

extern unsigned char L6010[] __asm__(".L6010");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);

void OvlFunc_924_2009420(void)
{
    struct Actor *a;
    int x;
    int y;
    int e;
    int f;

    x = __MapActor_GetActor(0xb)->f8 / 0x100000;
    y = __MapActor_GetActor(0xb)->f10 / 0x100000;
    __CutsceneStart();
    if (x == 5 && y == 0xd) {
        __MapActor_GetActor(0xb)->fc -= 0x20000;
        a = __MapActor_GetActor(0xb);
        a->f3c = __MapActor_GetActor(0xb)->fc;
        __CopyMapTiles(5, 2, 5, 0xb, 1, 1);
        __PlaySound(0xd9);
        __Func_8010560(L6010, 9, 7);
        e = 9;
        f = 0xa;
        __Func_8010704(9, 5, 1, 1, e, f);
        __SetFlag(0x874);
    }
    __CutsceneEnd();
}
