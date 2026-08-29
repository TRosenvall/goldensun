extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092950(int a, int b);

void OvlFunc_953_2009c6c(void)
{
    int e, f;

    if (__GetFlag(0x95 << 4)) {
        __CopyMapTiles(0x40, 0, 0x30, 5, 2, 2);
        e = 0x10;
        f = 8;
        __Func_8010704(0xe, 8, 2, 1, e, f);
    } else {
        __Func_8092950(0x10, 2);
        if (__GetFlag(0x962)) {
            e = 0xe;
            f = 0xb;
            __Func_8010704(0x1e, 0x16, 1, 2, e, f);
        }
    }
}
