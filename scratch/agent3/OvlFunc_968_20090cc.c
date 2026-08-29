extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_20090cc(void)
{
    int e1, f1;
    int e2, f2;

    __SetFlag(0x161);
    e1 = 0x17;
    f1 = 8;
    __Func_8010704(0x24, 8, 1, 3, e1, f1);
    __CopyMapTiles(0x24, 8, 0x17, 8, 1, 3);
    __CopyMapTiles(0x64, 8, 0x57, 8, 1, 3);
    e2 = 0x2e;
    f2 = 0x37;
    __Func_8010704(0x35, 0x37, 3, 3, e2, f2);
    __CopyMapTiles(0x35, 0x37, 0x2e, 0x37, 3, 3);
    __CopyMapTiles(0x75, 0x37, 0x6e, 0x37, 3, 3);
}
