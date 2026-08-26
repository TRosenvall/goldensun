extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

void OvlFunc_897_200a8dc(int which)
{
    int t;

    t = 2;
    if (which) {
        __CopyMapTiles(9, 0x2d, 0x41, 5, t, t);
        __CopyMapTiles(0xb, 0x2e, 0x43, 6, 1, t);
    } else {
        __CopyMapTiles(0x59, 2, 0x41, 5, t, t);
        __CopyMapTiles(0x66, 0x20, 0x43, 6, 1, t);
    }
    __Func_800fe9c();
}
