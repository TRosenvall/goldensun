extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

void OvlFunc_897_200a84c(int f)
{
    int one;
    int two;

    if (f != 0) {
        one = 1;
        __CopyMapTiles(8, 0x2f, 0x40, 7, one, one);
        two = 2;
        __CopyMapTiles(7, 0x30, 0x3f, 8, two, one);
        __CopyMapTiles(7, 0x31, 0x3f, 9, two, one);
    } else {
        one = 1;
        __CopyMapTiles(0x38, 0, 0x40, 7, one, one);
        __CopyMapTiles(0x38, 0, 0x3f, 8, one, one);
        __CopyMapTiles(0x38, 0, 0x3f, 9, 2, one);
        __CopyMapTiles(0x3a, 0x19, 0x40, 8, one, one);
    }
    __Func_800fe9c();
}
