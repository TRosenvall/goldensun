/* Cluster OvlFunc_883_200806c..OvlFunc_883_200806c extracted from goldensun/asm/overlays/rom_780898/ovl_30_a_a_a_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_780898/overlay.ld.
 *
 * FindEntityAtPosition. Scans entity slots 8..0x41 -- the map-object range,
 * above the party slots -- and returns the first whose position matches, or 0.
 * HEAD OF A 17-MEMBER FAMILY; it was parked for four rounds.
 *
 * FOUR THINGS ARE LOAD-BEARING, and they were found in this order by watching
 * where the diff moved rather than by guessing:
 *
 * 1. THE TABLE IS INDEXED, NOT WALKED. Written as a pointer incremented with
 *    `*p++`, gcc gives the position argument the cheaper register and the
 *    table the callee-saved one; the ROM does the reverse. Written as
 *    `tbl[i]` over a base at +0x14, gcc strength-reduces it to exactly the
 *    ROM's `add r1, #0x34` / `ldmia r1!` and the registers come out right.
 *    Indexing makes the table live from the top, which is what changes its
 *    allocation priority. Seven earlier attempts moved statements around and
 *    none of them touched this.
 *
 * 2. THE COUNTER IS INITIALISED BEFORE the x coordinate is computed.
 *
 * 3. EACH SHIFTED COMPARISON GOES INTO ITS OWN VARIABLE. `a >>= 16` shifts in
 *    place and gcc keeps one register; the ROM's `asr r2, r3, #0x10` writes a
 *    different register, which is a new value, not a modified one.
 *
 * 4. THE COUNTER IS UNSIGNED. The ROM ends the loop with `bls`, not `ble`.
 *    That was the last instruction to differ, and it is the one that would
 *    have been easiest to call a compiler quirk and give up on.
 *
 * THE ANNOTATION'S SIGNATURE IS WRONG, and all 17 members share it. It reads
 * "r1 = the entity to skip (the caller itself)", but r1 is overwritten by
 * `mov r1, r2` before it is ever read. There is no second argument.
 *
 * The axis comparison really is mismatched, as the annotation says: x and z at
 * whole-tile resolution (asr #20) but y at 1/16 (asr #16, with 0xffff added
 * first to round negatives toward zero). That is what keeps an object on a
 * ledge from blocking one on the floor below.
 */
struct Vec { int x, y, z; };
struct Ent { unsigned char pad_00[8]; int x, y, z; };

extern unsigned char iwram_3001ebc[];

struct Ent *OvlFunc_883_200806c(struct Vec *pos)
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
