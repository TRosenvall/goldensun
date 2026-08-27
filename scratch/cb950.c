extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_943_200b950(void)
{
    int one;
    int five;
    int e;
    int f;

    one = 1;
    five = 5;
    __CopyMapTiles(0x4e, 0x27, 0x4e, 0x28, five, one);
    __CopyMapTiles(0x4e, 0x27, 0x4e, 0x29, five, one);
    __CopyMapTiles(0x4e, 0x27, 0x4f, 0x2a, 4, one);
    __CopyMapTiles(0x4e, 0x27, 0x52, 0x2b, one, one);
    e = 0x11;
    f = 0x28;
    __Func_8010704(0x11, 0x26, 5, 2, e, f);
}
