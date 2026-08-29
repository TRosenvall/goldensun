struct Actor {
    unsigned char pad0[0x23];
    unsigned char flags;
};

extern unsigned char gState[];
extern char *iwram_3001e70;

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_200ae84(void);
extern void OvlFunc_932_200ba44(void);
extern void OvlFunc_932_200b460(int n);
extern void OvlFunc_932_200abb0(int a, int b, int c, int d);

void OvlFunc_932_200a490(void)
{
    char *w;
    struct Actor *a;
    int off;
    int s1, s2;

    if (__GetFlag(0x907)) {
        w = iwram_3001e70;
        *(unsigned short *)(w + 0x14) = 0xfdff & *(unsigned short *)(w + 0x14);
        __MapActor_SetPos(0xa, 0, 0);
    } else {
        off = 0xe1 << 1;
        if (__GetFlag(0x109) == 0 && *(short *)(gState + off) == 0x63)
            OvlFunc_932_200ae84();
        OvlFunc_932_200ba44();
        if (__GetFlag(0x907) == 0) {
            __Func_8092950(0xa, 2);
            __MapActor_SetAnim(0xa, 3);
            OvlFunc_932_200abb0(0xbb << 18, 0x80 << 12, 0x8c << 17, 0x80 << 8);
        }
    }
    OvlFunc_932_200b460(9);
    if (__GetFlag(0x80 << 2)) {
        __MapActor_SetAnim(9, 5);
        s1 = 0x19;
        s2 = 0xd;
        __Func_8010704(0x17, 0xd, 1, 1, s1, s2);
        a = __MapActor_GetActor(9);
        a->flags |= 2;
    }
    if (__GetFlag(0x325)) {
        s1 = 0xb;
        s2 = 0x49;
        __Func_8010704(0xa, 0x48, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x31, 0x20, 0xb, 4, s1, s2);
    } else {
        s1 = 0xb;
        s2 = 0x49;
        __Func_8010704(0xc, 0x48, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0x30, 0x20, 0xb, 4, s1, s2);
    }
}
