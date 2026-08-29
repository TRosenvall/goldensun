extern unsigned char gState[];
extern char *iwram_3001e70;

extern int __GetFlag(int id);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_200b460(int n);

void OvlFunc_932_200a5c0(void)
{
    unsigned char *g;
    unsigned char *p;
    char *w;
    int s1, s2;
    int x, y;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        x = 0xb3 << 17;
        y = 0xd0 << 15;
        if (__GetFlag(0x109) == 0)
            __MapActor_SetPos(8, x, y);
    }
    OvlFunc_932_200b460(9);
    if (__GetFlag(0x80 << 2)) {
        p = (unsigned char *)__MapActor_GetActor(9);
        __MapActor_SetAnim(9, 5);
        s1 = 0x2b;
        s2 = 0x29;
        __Func_8010704(0x2d, 0x29, 1, 1, s1, s2);
        p += 0x23;
        *p = 2 | *p;
    }
    if (__GetFlag(0x907)) {
        w = iwram_3001e70;
        *(unsigned short *)(w + 0x14) = 0xfdff & *(unsigned short *)(w + 0x14);
    }
    if (__GetFlag(0x326)) {
        s1 = 0x10;
        s2 = 0x5c;
        __Func_8010704(0x11, 0x5d, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x2e, 0x1d, 0x10, 0x1c, s1, s2);
    } else {
        s1 = 0x10;
        s2 = 0x5c;
        __Func_8010704(0xf, 0x5d, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x2f, 0x1d, 0x10, 0x1c, s1, s2);
    }
}
