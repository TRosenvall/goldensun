/* Cluster OvlFunc_968_200832c..OvlFunc_968_200832c extracted from
 * goldensun/asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_a.s.
 *
 * Total .text for this TU = 72 bytes (= 0x48).
 * Placed in the run in goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * Scans entries 8..0x41 of the actor table for the one standing on the same
 * tile as the caller -- all three coordinates compared at tile resolution,
 * `>> 20`. Returns the entry, or 0.
 *
 * TWO STATEMENT SWAPS, AND NOTHING ELSE. The body was exact on the first
 * screen; the nine differing lines were all in the six-instruction preamble,
 * and both fixes are orderings that say nothing about behaviour:
 *
 *   the block pointer is read into its own local BEFORE the caller's first
 *   coordinate, which is what puts the caller in r4 and the block in r2;
 *   the loop counter is initialised BEFORE the coordinate shift, not after.
 *
 * The second one is worth noticing: `i = 8;` and `qx = q->x >> 20;` are
 * independent, and writing them in the other order moves the `mov r5, #8` past
 * the `asr`. Two instructions, decided entirely by which line comes first.
 *
 * The counter is UNSIGNED -- the ROM's `cmp r5, #0x41 / bls` says so, and an
 * `int` gives `ble`. Third function in three batches to turn on that.
 */

struct Q {
    int x;
    int y;
    int z;
};

struct E {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
};

extern char *iwram_3001ebc;

struct E *OvlFunc_968_200832c(struct Q *q)
{
    char *blk;
    struct E **p;
    struct E *e;
    unsigned int i;
    int qx;

    blk = iwram_3001ebc;
    i = 8;
    qx = q->x >> 20;
    p = (struct E **)(blk + 0x34);
    do {
        e = *p;
        p++;
        if (qx == (e->x >> 20)
            && (q->y >> 20) == (e->y >> 20)
            && (q->z >> 20) == (e->z >> 20))
            return e;
        i++;
    } while (i <= 0x41);
    return 0;
}
