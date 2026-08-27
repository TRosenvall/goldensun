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
    int scales[8];
};

extern struct Blk *iwram_3001f2c;
extern int _GetPartySize(void);
extern void _UpdateSprite(struct Sprite *s, int *pos, int *scale, int mode);

void Func_80a19a0(void)
{
    struct Blk *b;
    struct Sprite *p;
    int scale[2];
    int pos[4];
    int i;
    int n;
    int yy;

    b = iwram_3001f2c;
    n = (unsigned short)_GetPartySize();
    for (i = 0; i < n; i++) {
        yy = 0x1e20000 - (b->ys[i] << 16);
        p = b->actors[i];
        if (p != 0) {
            p->b2 = 0;
            scale[0] = b->scales[i];
            scale[1] = b->scales[i];
            pos[0] = b->xs[i] << 16;
            pos[1] = yy;
            pos[2] = (b->ys[i] << 16) + yy;
            pos[3] = 0;
            _UpdateSprite(p, pos, scale, 0x80 << 7);
        }
    }
}
