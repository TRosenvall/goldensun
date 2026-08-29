/* OvlFunc_959_2009a44  --  0x02009a44
 *
 * Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a_c_c_c.s.
 *
 * The same trigger shape as OvlFunc_959_20099e8 with a facing test added:
 * `facing == 0xc000 || facing == 0x4000`, read off the ROMs two compares where
 * the first jumps INTO the body and the second jumps out.
 *
 * Both facing constants are written as shifts (`0xc0 << 8`, `0x80 << 7`)
 * because that is how the ROM builds them; as plain 0xc000 they would be pool
 * loads.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    unsigned char pad0c[4];
    int z;
};

extern char *iwram_3001ebc;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void OvlFunc_959_2008e80(void);

void OvlFunc_959_2009a44(void)
{
    struct Actor *a;
    char *p;
    int x;
    int z;
    int v;

    v = 0x28;
    a = __MapActor_GetActor(0);
    if (__GetFlag(0xd6 << 2) == 0) {
        x = a->x / 0x100000;
        z = a->z / 0x100000;
        if (x == 0x10 && z > 0x37 && z <= 0x3a
            && (a->facing == (0xc0 << 8) || a->facing == (0x80 << 7))) {
            p = iwram_3001ebc;
            *(short *)(p + (0xb6 << 1)) = v;
            OvlFunc_959_2008e80();
        }
    }
}
