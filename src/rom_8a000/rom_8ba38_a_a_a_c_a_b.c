/* Func_808bd24 -- 0x0808bd24
 *
 * Reads the terrain byte under the party leader: take the leader's position,
 * push it along its facing, then look the result up in whichever of two tile
 * grids the current map mode selects.
 *
 * SEVEN LEVERS, and two of them are worth reading even if you never touch this
 * function again.
 *
 *  - A NAMED BYTE OFFSET BLOCKS REASSOCIATION. `off = idx * 4 + 0x14;` then
 *    `*(struct A **)(p + off)` gives the ROM's `lsl #2 / add #0x14 /
 *    ldr [r6, r2]`. Written as one expression gcc folds the 0x14 into the
 *    load's displacement instead and the add disappears.
 *
 *  - STATEMENT ORDER DECIDES REGISTER NAMING. The iwram pointer must be
 *    fetched BEFORE gState. Reversing those two lines swaps r2 and r3 across
 *    the whole prologue: 24 differing against 12, for a reordering that changes
 *    nothing semantically.
 *
 * The rest are on file: the second global is DERIVED from the first at a fixed
 * distance (`*(unsigned char **)((char *)&iwram_3001ebc - 0x4c)`), which is the
 * only spelling that produces the ROM's `sub r3, #0x4c` -- two separate externs
 * would pool two addresses; gState needs a named base per use site; the entity
 * is read through a typed struct rather than casts, which fixed the scheduling
 * of the third store against the call's argument setup (12 differing to 6); and
 * both `/ 0x200000` and `/ 0x100000` are SIGNED DIVISIONS, not shifts -- the
 * `cmp / bge / ldr =0x1fffff / add / asr #21` shape with the bias in the pool.
 *
 * THE LAST SIX LINES WERE ONE SPELLING. `t = base; t += ...;` rather than a
 * separate `base` local is what gives the ROM's destructive `add r1, r3` and
 * puts the result in r1. Inlining the base entirely is much worse (22
 * differing) because it also sinks the base load past the divisions.
 *
 * Verified with tools/objcmp.py: 188 bytes, 86 encodings and 4 relocations
 * identical.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char ewram_2020000[];
extern void vec3_translate(int a, int b, int *v);

struct A {
    unsigned char pad00[6];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
};

int Func_808bd24(void)
{
    int v[3];
    int *bp;
    unsigned char *g;
    unsigned char *p;
    unsigned char *q;
    struct A *e;
    unsigned char *t;
    int off;

    p = iwram_3001ebc;
    g = gState;
    off = *(int *)(g + (0xfa << 1)) * 4 + 0x14;
    e = *(struct A **)(p + off);
    q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
    if (e == 0)
        return 0;
    bp = v;
    bp[0] = e->f08;
    bp[1] = e->f0c;
    bp[2] = e->f10;
    vec3_translate(0x80 << 13, e->f06, bp);
    if (*(short *)(p + (0xcf << 1)) == 3) {
        t = ewram_2020000 + (((bp[0] / 0x200000) & 0x1f) + (((bp[2] / 0x200000) & 0x1f) << 5)) * 4;
    } else {
        t = *(unsigned char **)(q + (0x98 << 1));
        t += ((bp[0] / 0x100000) + ((bp[2] / 0x100000) << 7)) * 4;
    }
    return t[2];
}
