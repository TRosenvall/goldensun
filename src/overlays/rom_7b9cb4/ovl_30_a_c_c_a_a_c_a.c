/* OvlFunc_932_2008a94
 *
 * Cut out of goldensun/asm//overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_c_a.s.
 *
 * The sibling of 20089ec for the second bridge.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_932_2008a94(void)
{
    int m;
    int n;
    int p;
    int q;

    if (__GetFlag(0x325) != 0) {
        m = 0xb;
        n = 0x49;
        __Func_8010704(0xc, 0x48, 1, 1, m, n);
        p = 1;
        q = 2;
        __CopyMapTiles(0x30, 0x20, 0xb, 4, p, q);
        __ClearFlag(0x325);
    } else {
        m = 0xb;
        n = 0x49;
        __Func_8010704(0xa, 0x48, 1, 1, m, n);
        p = 1;
        q = 2;
        __CopyMapTiles(0x31, 0x20, 0xb, 4, p, q);
        __SetFlag(0x325);
    }
}
