struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x3c - 0x14];
    int f3c;
};

extern unsigned char L6064[] __asm__(".L6064");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);

void OvlFunc_924_2009bf0(void)
{
    struct Actor *a;
    int x;
    int y;
    int f;

    x = __MapActor_GetActor(8)->f8 / 0x100000;
    y = __MapActor_GetActor(8)->f10 / 0x100000;
    __CutsceneStart();
    if (x == 0xa && y == 0x17) {
        __MapActor_GetActor(8)->fc -= 0x20000;
        a = __MapActor_GetActor(8);
        a->f3c = __MapActor_GetActor(8)->fc;
        __CopyMapTiles(6, 0x1d, 0xa, 0x17, 1, 1);
        __PlaySound(0xd9);
        __Func_8010560(L6064, 0xa, 0x12);
        f = 0x13;
        __Func_8010704(0xa, 0x10, 1, 1, x, f);
        __SetFlag(0x878);
    }
    __CutsceneEnd();
}
