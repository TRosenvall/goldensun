extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);

void OvlFunc_911_200a910(void)
{
    int s0, s1, s2, s3, s4, s5;

    if (__GetFlag(0x845) == 0) {
        __MapActor_SetPos(8, 0, 0);
        s0 = 9;
        s1 = 0x12;
        __Func_8010704(9, 0x11, 5, 1, s0, s1);
        __Func_800fe9c();
        __WaitFrames(1);
    } else {
        __MapActor_SetPos(9, 0, 0);
    }
    if (__GetFlag(0x847)) {
        s2 = 5;
        s3 = 7;
        __CopyMapTiles(0x5b, 0x13, 0x48, 9, s2, s3);
        s4 = 8;
        s5 = 0xb;
        __Func_8010704(0x17, 0xb, 5, 7, s4, s5);
        __Func_800fe9c();
        __WaitFrames(1);
    }
}
