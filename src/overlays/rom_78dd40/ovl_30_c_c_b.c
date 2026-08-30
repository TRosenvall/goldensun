/* Cluster OvlFunc_893_2008054..OvlFunc_893_2008054 split out of goldensun/asm/overlays/rom_78dd40/ovl_30_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * A STRUCT POINTER IS A REGISTER-ALLOCATION LEVER, not just a readability
 * choice. The previous park blamed constant-CSE across the three equal
 * arguments; that was already solved by the three-named-locals lever. The real
 * residue was a five-instruction register swap (base in r2 where the ROM has
 * r1) that survived THIRTEEN pointer-and-offset spellings. The struct form got
 * it on the first screen. Check docs/structs.md for a name before spending
 * screens on offset arithmetic -- iwram_3001ebc was already pinned there.
 *
 * OvlFunc_894_2008054 is a byte-identical twin of this function.
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

int OvlFunc_893_2008054(void)
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
