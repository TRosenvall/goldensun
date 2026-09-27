/* Func_80ad35c  --  0x080ad35c, split out of asm/rom_a1000/rom_ad274_c_a_a.s;
 * Func_80ad40c, 80ad508 and 80ad5b4 follow in the _c_* pieces. Matched from
 * scratch.
 *
 * ALIAS SET 0 ON ONE LOAD, WHEN THE FLAG GETS EVERYTHING BUT ONE PAIR. With
 * -fno-strict-aliasing this was 2 off (the bitfield strb and the scale[0] stack
 * store pinned in source order, the ROM has them swapped); strict, at best 6
 * (the int store pos[1] sinks below the short load of ys2, which the ROM keeps
 * after it) -- all 5040 and 40320 statement orders tried by script. Reading
 * ONLY ys2 through a union member gives that one load alias set 0 (c-common's
 * union rule), so under strict aliasing it alone conflicts with the earlier int
 * store. The mirror of the union-STORE trick in rom_ad274_a.c: when the flag
 * fixes one reorder and breaks another, route only the access the ROM keeps
 * ordered through a union. This TU must NOT go on ALIAS_CFLAGS.
 *
 * yy reads b->ys[i] at +0x144, not ys2; the ROM does exactly that.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
};

struct Blk {
    unsigned char pad000[0x114];
    struct Sprite *actors[8];
    short xs[8];
    short ys[8];
    unsigned char pad154[0x224 - 0x154];
    struct Sprite *actors2[4];
    short xs2[4];
    union {
        short ys2[4];
        int w;
    } cy;
};

extern struct Blk *iwram_3001f2c;
extern void _UpdateSprite(struct Sprite *s, int *pos, int *scale, int mode);

void Func_80ad35c(void)
{
    struct Blk *b;
    struct Sprite *p;
    int scale[2];
    int pos[4];
    int i;
    int yy;

    b = iwram_3001f2c;
    for (i = 0; i < 4; i++) {
        p = b->actors2[i];
        if (p != 0) {
            yy = 0x1e20000 - (b->ys[i] << 16);
            p->b2 = 0;
            scale[0] = 0x10000;
            scale[1] = 0x10000;
            pos[0] = b->xs2[i] << 16;
            pos[1] = yy;
            pos[2] = (b->cy.ys2[i] << 16) + yy;
            pos[3] = 0;
            _UpdateSprite(p, pos, scale, 0x80 << 7);
        }
    }
}
