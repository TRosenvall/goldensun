// fakematch
/* OvlFunc_919_200805c  --  0x0200805c
 *
 * Was goldensun/asm/overlays/rom_7a67d8/ovl_30_a_c_c_c_a_a.s, which held it
 * alone.
 *
 * Nudges two actors by the same delta: the one the map's current-actor slot
 * names, and the one parked at iwram_3001ebc + 0x1e0. Each gets its two
 * coordinate words advanced, then its facing recomputed from the byte at +0x22
 * and written to both +0xc and +0x14.
 *
 * PARKED AT 6 OF 56 AS AN ALLOCATION-ORDER CASE. THE PARK WAS PREMATURE.
 *
 * The residue was a two-register rotation: the ROM holds the first loaded
 * coordinate in r1, we held it in r3, and the shape repeated in both bodies.
 * Six spellings tied at exactly 6 -- declaration order of the two locals both
 * ways, the two loads issued in either order, the +0x22 byte read before or
 * after the stores -- and on that evidence it was written up as the allocator.
 *
 * Every one of those six varied ORDER. None of them tried a REGISTER PIN. That
 * was the structural assumption they all shared, and pinning the two loaded
 * coordinates to r1 and r2 matches:
 *
 *     register int pu __asm__("r1");
 *     register int pv __asm__("r2");
 *     pu = a->f8;  pv = a->f10;
 *     pu += dx;    pv += dy;
 *     a->f8 = pu;  a->f10 = pv;
 *
 * THE PIN IDIOM IS NOT ONLY FOR CONSTANTS. Every previous use in this tree
 * pinned a literal or a call argument; here it pins a value LOADED FROM MEMORY,
 * to place the load's destination rather than to defeat a hoist. Nothing about
 * the idiom required constants, but the habit had made "register rotation" and
 * "unreachable" look like the same thing.
 *
 * The three levers the park did record still stand and are still needed: the
 * gState base must be a named local; both coordinate loads must be named so
 * they issue before either add; and the adds are compound assignments, since
 * the ROM's `add r1, r6` is two-address. Those took 51 of 56 down to 6. The pin
 * takes the last six.
 */

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x22 - 0x18];
    unsigned char f22;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __Func_8011f54(int a);

void OvlFunc_919_200805c(int dx, int dy)
{
    unsigned char *blk;
    unsigned char *g;
    struct Actor *a;
    int r;
    int u;
    int v;

    blk = iwram_3001ebc;
    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    dx <<= 20;
    dy <<= 20;
    if (a != 0) {
        {
            register int pu __asm__("r1");
            register int pv __asm__("r2");
            pu = a->f8;
            pv = a->f10;
            pu += dx;
            pv += dy;
            a->f8 = pu;
            a->f10 = pv;
        }
        r = __Func_8011f54(a->f22);
        a->fc = r;
        a->f14 = r;
    }
    a = *(struct Actor **)(blk + (0xf0 << 1));
    if (a != 0) {
        {
            register int pu __asm__("r1");
            register int pv __asm__("r2");
            pu = a->f8;
            pv = a->f10;
            pu += dx;
            pv += dy;
            a->f8 = pu;
            a->f10 = pv;
        }
        r = __Func_8011f54(a->f22);
        a->fc = r;
        a->f14 = r;
    }
}
