extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_890_2008238(void)
{
    if (__GetFlag(0x80b))
        __CopyMapTiles(0x2d, 0x1c, 0x22, 0xa, 2, 1);
    if (__GetFlag(0x80c))
        __CopyMapTiles(0x2f, 0x1c, 0x24, 0xa, 2, 1);
    if (__GetFlag(0x80d))
        __CopyMapTiles(0x2d, 0x1d, 0x22, 0xb, 2, 1);
    if (__GetFlag(0x80e))
        __CopyMapTiles(0x2f, 0x1d, 0x24, 0xb, 2, 1);
}
