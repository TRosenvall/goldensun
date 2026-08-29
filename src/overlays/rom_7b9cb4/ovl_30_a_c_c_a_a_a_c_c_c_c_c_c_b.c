/* OvlFunc_932_20089ec
 *
 * Cut out of goldensun/asm//overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_b.s.
 *
 * Toggles the bridge tiles and the flag together. The two stack-argument pairs
 * are assigned INSIDE each arm rather than hoisted -- the batch-52 shape.
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
