extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_890_2008238(void)
{
    int e5, e6;

    e5 = 2;
    e6 = 1;
    if (__GetFlag(0x80b))
        __CopyMapTiles(0x2d, 0x1c, 0x22, 0xa, e5, e6);
    if (__GetFlag(0x80c))
        __CopyMapTiles(0x2f, 0x1c, 0x24, 0xa, e5, e6);
    if (__GetFlag(0x80d))
        __CopyMapTiles(0x2d, 0x1d, 0x22, 0xb, e5, e6);
    if (__GetFlag(0x80e))
        __CopyMapTiles(0x2f, 0x1d, 0x24, 0xb, e5, e6);
}
