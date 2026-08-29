/* Cluster OvlFunc_common1_2018..OvlFunc_common1_2018 extracted from
 * goldensun/asm/overlays/common/common1_c_c.s.
 *
 * Total .text for this TU = 72 bytes (= 0x48).
 * First in the run, ahead of the _b piece, in the THREE overlay.ld scripts that
 * name this object; the _b piece keeps the .data, .data1 and .bss sections.
 *
 * BYTE-IDENTICAL TWIN of OvlFunc_968_200832c in overlays/rom_7f2f14; this C is
 * that file's verbatim, with only the symbol changed. See it for the two
 * statement swaps in the preamble and the unsigned loop counter.
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

struct E *OvlFunc_common1_2018(struct Q *q)
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
