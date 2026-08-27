struct Actor {
    unsigned char pad0[0xa];
    short fa;
    int fc;
    unsigned char pad10[2];
    short f12;
    int f14;
    unsigned char pad18[0x55 - 0x18];
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);

void OvlFunc_968_20096a4(void)
{
    struct Actor *act;
    int x;
    int z;
    unsigned char *p;
    int e1;
    int f1;
    int e2;
    int f2;

    act = __MapActor_GetActor(0);
    x = act->fa;
    z = act->f12;
    if (x < 0x2a4 || x > 0x2ab || z < 0x314 || z >= 0x31c) {
        __CopyMapTiles(0x35, 0x32, 0x2a, 0x31, 1, 1);
        e1 = 3;
        f1 = 5;
        __CopyMapTiles(0x37, 0x75, 0x29, 0x75, e1, f1);
        __ClearFlag(0x201);
        act->f55 |= 1;
        act->f14 = 0;
        act->fc = 0;
    } else if (__GetFlag(0x201) == 0) {
        __CutsceneStart();
        __CutsceneWait(5);
        __CopyMapTiles(0x34, 0x32, 0x2a, 0x31, 1, 1);
        e2 = 3;
        f2 = 5;
        __CopyMapTiles(0x34, 0x75, 0x29, 0x75, e2, f2);
        __SetFlag(0x201);
        __PlaySound(0xa1);
        p = &act->f55;
        *p &= 0xfe;
        act->f14 = 0xfffe0000;
        act->fc = 0xfffe0000;
        __CutsceneEnd();
    }
}
