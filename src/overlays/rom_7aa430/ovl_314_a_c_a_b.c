/* Cluster OvlFunc_923_2008350..OvlFunc_923_2008350 extracted from goldensun/asm/overlays/rom_7aa430/ovl_314_a_c_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * FindEntityAtPosition, one of sixteen identical copies -- one per overlay,
 * byte-for-byte the same body. See
 * src/overlays/rom_780898/ovl_30_a_a_a_c_b.c for the four things that are
 * load-bearing here; three of them look like style and are not.
 *
 * The annotation on every member claims a second argument that r1 is
 * overwritten before reading. There is one argument.
 */
struct Vec { int x, y, z; };
struct Ent { unsigned char pad_00[8]; int x, y, z; };

extern unsigned char iwram_3001ebc[];

struct Ent *OvlFunc_923_2008350(struct Vec *pos)
{
    struct Ent **tbl;
    struct Ent *e;
    char *base;
    unsigned int i;
    int a, b, px, ay, by;

    base = *(char **)iwram_3001ebc;
    tbl = (struct Ent **)(base + 0x14);
    i = 8;
    px = pos->x >> 20;
    for (; i <= 0x41; i++) {
        e = tbl[i];
        if (px == (e->x >> 20)) {
            a = pos->y;
            if (a < 0)
                a += 0xffff;
            ay = a >> 16;
            b = e->y;
            if (b < 0)
                b += 0xffff;
            by = b >> 16;
            if (ay == by) {
                if ((pos->z >> 20) == (e->z >> 20))
                    return e;
            }
        }
    }
    return 0;
}
