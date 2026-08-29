extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern void __Func_8091ff0(int n);

void OvlFunc_936_20098a4(void)
{
    char *p;
    unsigned char *g;
    int two;
    int s0;
    int s1;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x204;
    if (__GetFlag(0x915)) {
        s1 = 3;
        two = 2;
        __CopyMapTiles(0x3a, 5, 0x3a, 8, two, s1);
        s0 = 8;
        s1 = 0xa;
        __Func_8010704(8, 0xb, 2, 1, s0, s1);
        s1 = 1;
        __CopyMapTiles(8, 0xc, 8, 0xb, two, s1);
        __Func_800fe9c();
        __WaitFrames(1);
    }
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) <= 3)
        __Func_8091ff0(0xaa);
}
