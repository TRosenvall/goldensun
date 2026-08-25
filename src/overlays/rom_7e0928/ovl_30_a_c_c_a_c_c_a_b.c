/* Cluster OvlFunc_956_2008204..OvlFunc_956_2008204 extracted from
 * goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a.s.
 *
 * Total .text for this TU = 58 bytes (= 0x3a).
 * First in the run, ahead of the _b piece, in goldensun/overlays/rom_7e0928/overlay.ld.
 *
 * Nudges two actors back by 0xcccc on the x axis when the fetched actor's
 * type is one of four consecutive values.
 *
 * THE RANGE TEST IS THE FUSED FORM AND THAT IS WHAT THE ROM WANTS.
 * `(unsigned)(v - 0xb7) <= 3` gives `sub r3, #0xb7 / cmp r3, #3 / bhi`, which
 * is exactly the ROM. docs/elevation.md records the opposite lever -- splitting
 * a compound condition into separate statements to STOP the fusion -- for the
 * cases where the ROM does two `cmp`s instead. Check which form the ROM has
 * before reaching for either.
 *
 * THE STATEMENT ORDER SETS WHICH GLOBAL IS READ FIRST. The block pointer has to
 * be read before gState: written the other way round gcc reads gState first and
 * then derives the block's 0x1e0 from gState's 0x1f4 with `sub r2, #0x14`,
 * which is one instruction the ROM does not have.
 */

struct Actor {
    unsigned char pad00[8];
    int x8;
    unsigned char pad0c[6];
    short f12;
};

typedef struct {
    unsigned char pad[0x1f4];
    struct Actor *who;
} GlobalState;

typedef struct {
    unsigned char pad[0x1e0];
    struct Actor *a;
} Blk;

extern GlobalState gState;
extern Blk *iwram_3001ebc;
extern struct Actor *__MapActor_GetActor(struct Actor *w);

void OvlFunc_956_2008204(void)
{
    Blk *b;
    struct Actor *p;
    struct Actor *q;
    struct Actor *r;

    b = iwram_3001ebc;
    q = gState.who;
    p = b->a;
    r = __MapActor_GetActor(q);
    if ((unsigned int)(r->f12 - 0xb7) <= 3) {
        p->x8 += -0xcccc;
        r->x8 += -0xcccc;
    }
}
