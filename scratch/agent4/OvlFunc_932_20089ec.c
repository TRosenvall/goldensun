extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_932_20089ec(void)
{
    int m;
    int n;
    int p;
    int q;

    if (__GetFlag(0x323) != 0) {
        m = 0x18;
        n = 0x50;
        __Func_8010704(2, 0, 1, 1, m, n);
        p = 1;
        q = 2;
        __CopyMapTiles(2, 1, 0x18, 0xb, p, q);
        __ClearFlag(0x323);
    } else {
        m = 0x18;
        n = 0x50;
        __Func_8010704(0, 0, 1, 1, m, n);
        p = 1;
        q = 2;
        __CopyMapTiles(0, 1, 0x18, 0xb, p, q);
        __SetFlag(0x323);
    }
}
