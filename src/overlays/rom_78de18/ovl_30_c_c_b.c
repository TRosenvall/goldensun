/* Cluster OvlFunc_894_2008054..OvlFunc_894_2008054 split out of goldensun/asm/overlays/rom_78de18/ovl_30_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * Byte-identical twin of OvlFunc_893_2008054 in overlay rom_78dd40; one
 * solution covered both. See that file for why the struct pointer is the
 * lever rather than offset arithmetic.
 */
struct Blk {
    unsigned char pad000[0x1c0];
    unsigned int stepDelay;
    unsigned char pad1c4[4];
    unsigned int msgDelay;
};

extern struct Blk *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __Func_8091ff0(int a);
extern void __Func_8012330(int x, int y, int z);
extern void __StartEarthquake(void);

int OvlFunc_894_2008054(void)
{
    struct Blk *b;
    int x;
    int y;
    int z;

    x = 0x80 << 9;
    y = 0x80 << 9;
    z = 0x80 << 9;
    b = iwram_3001ebc;
    b->stepDelay = 0x204;
    b->msgDelay = 0x10;
    if (__GetFlag(0x814)) {
        __Func_8091ff0(0x8d);
        __Func_8012330(x, y, z);
        __StartEarthquake();
    }
    return 0;
}
