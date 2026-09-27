/* Func_8010d48  --  0x08010d48, split out of asm/rom_9000/rom_108e4.s (ten
 * functions; the rest stay in _a.s/_c.s). Matched from scratch.
 *
 * - Shift the arguments IN PLACE -- `tz >>= 4; tx >>= 4; z >>= 3; x >>= 3;` --
 *   rather than inside the expressions: 107 differing -> 7.
 * - `state` and the z sign bit tie at 3 references each over 30 and 31 insns;
 *   that one-instruction gap in live length decides which gets r14 and which
 *   r8 (local-alloc's priority formula). Moving `cz >>= 24; cx >>= 24;` above
 *   the table store flips it.
 */
extern unsigned char *iwram_3001e70;
extern int Func_80108e4(int layer, int qx, int qz, int tileset, int force);

struct WorldMapState {
    int *pos;
    unsigned char pad[0x134];
    unsigned short quad[256];
};

void Func_8010d48(int tx, int tz, int x, int z)
{
    struct WorldMapState *state = (struct WorldMapState *)iwram_3001e70;
    int *p = state->pos;
    int cx = 0, cz = 0;
    int id;
    int d;

    if (p) {
        cx = *p++;
        cz = p[1];
    }
    cz >>= 24;
    cx >>= 24;
    tz >>= 4;
    tx >>= 4;
    z >>= 3;
    x >>= 3;
    id = (tz << 4) + tx;
    state->quad[(((z / 2) & 15) << 4) + ((x / 2) & 15)] = id;
    d = cx - x;
    if (d >= 0) {
        if (d > 1)
            return;
    } else if (x - cx > 1)
        return;
    d = cz - z;
    if (d >= 0) {
        if (d > 1)
            return;
    } else if (z - cz > 1)
        return;
    Func_80108e4(0, x / 2, z / 2, id, 1);
    Func_80108e4(1, x / 2, z / 2, id + 0x140, 1);
}
