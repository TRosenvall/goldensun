extern unsigned char gState[];
extern char *iwram_3001e70;

extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_932_200ba44(void);
extern void OvlFunc_932_200ae1c(void);
extern void OvlFunc_932_200abb0(int a, int b, int c, int d);

void OvlFunc_932_200a310(void)
{
    char *w;
    int s1, s2;

    if (__GetFlag(0x8fe)) {
        w = iwram_3001e70;
        *(unsigned short *)(w + 0x14) = 0xfdff & *(unsigned short *)(w + 0x14);
        __MapActor_SetPos(9, 0, 0);
    } else {
        OvlFunc_932_200ba44();
        if (__GetFlag(0x109) == 0 && *(short *)(gState + (0xe1 << 1)) == 0x63) {
            OvlFunc_932_200ae1c();
        } else {
            s1 = 0x25;
            s2 = 0x18;
            __Func_8010704(0x26, 0x18, 1, 2, s1, s2);
            s1 = 0x2d;
            s2 = 0x17;
            __Func_8010704(0x2c, 0x17, 1, 2, s1, s2);
            if (__GetFlag(0x8fe) == 0) {
                __Func_8092950(9, 2);
                __MapActor_SetAnim(9, 3);
                OvlFunc_932_200abb0(0xee << 16, 0, 0xd1 << 17, 0x80 << 8);
            }
        }
    }
    if (__GetFlag(0x323)) {
        s1 = 0x18;
        s2 = 0x50;
        __Func_8010704(0, 0, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(0, 1, 0x18, 0xb, s1, s2);
    } else {
        s1 = 0x18;
        s2 = 0x50;
        __Func_8010704(2, 0, 1, 1, s1, s2);
        s1 = 1;
        s2 = 2;
        __CopyMapTiles(2, 1, 0x18, 0xb, s1, s2);
    }
}
