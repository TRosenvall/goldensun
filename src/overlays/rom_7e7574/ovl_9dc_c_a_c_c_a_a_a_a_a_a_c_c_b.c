/* OvlFunc_959_20099e8  --  0x020099e8
 *
 * Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a_c_c_b.s.
 *
 * A trigger tile: if the player is standing on one particular square facing
 * nothing in particular, start the scene.
 *
 * The stored 0x29 is an int local declared at the top -- the HImode rule. As a
 * literal through a short * it is a pool load where the ROM has mov r2, #0x29,
 * and the two nested ifs supply the dominating block the rule needs.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern char *iwram_3001ebc;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void OvlFunc_959_2008f30(void);

void OvlFunc_959_20099e8(void)
{
    struct Actor *a;
    char *p;
    int x;
    int z;
    int v;

    v = 0x29;
    a = __MapActor_GetActor(0);
    if (__GetFlag(0x35b) == 0) {
        x = a->x / 0x100000;
        z = a->z / 0x100000;
        if (x == 0x2b && z > 0x1c && z <= 0x1f) {
            p = iwram_3001ebc;
            *(short *)(p + (0xb6 << 1)) = v;
            OvlFunc_959_2008f30();
        }
    }
}
