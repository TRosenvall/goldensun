extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_890_20083f4(void)
{
    if (__GetFlag(0x826)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1e, 0x22, 0xa, e5, e6);
    }
    if (__GetFlag(0x827)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1e, 0x24, 0xa, e5, e6);
    }
    if (__GetFlag(0x828)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1f, 0x22, 0xb, e5, e6);
    }
    if (__GetFlag(0x829)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1f, 0x24, 0xb, e5, e6);
    }
}
