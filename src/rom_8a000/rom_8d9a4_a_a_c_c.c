/* Func_808df1c  --  0x0808df1c, was asm/rom_8a000/rom_8d9a4_a_a_c_c.s (this
 * function alone), so it converts whole. Matched on the first compile from its
 * landed sibling Func_808ddec's idioms.
 */
struct Ent {
    unsigned char pad00[6];
    unsigned short f06;
    int x;
    int y;
    int z;
    unsigned char pad14[0x59 - 0x14];
    unsigned char f59;
};

extern struct Ent *GetFieldActor(int id);
extern int Func_808ddb8(int mode);
extern int Func_8000948(int v);
extern int atan2(int dz, int dx);

int Func_808df1c(int self, int mode)
{
    struct Ent *a;
    struct Ent *b;
    unsigned char *fl;
    int best = -1;
    int bestd;
    int i;
    int dx, dz, d;
    int px, pz;
    int ang, diff, dir, lim, range;
    int (*fp)(int);

    bestd = Func_808ddb8(mode);
    a = GetFieldActor(self);
    if (a != 0) {
        dir = (a->f06 + 0x2000) & 0xc000;
        for (i = 0; i <= 0x42; i++) {
            if (i == self)
                continue;
            b = GetFieldActor(i);
            if (b == 0)
                continue;
            fl = &b->f59;
            if (*fl & 8)
                continue;
            range = 0x80000;
            if (mode == 0xd)
                range = 0x300000;
            if (mode == 5)
                range = 0x400000;
            if (mode == 2)
                range = 0x100000;
            if (b->y - a->y >= 0) {
                if (b->y - a->y > range)
                    continue;
            } else {
                if (a->y - b->y > range)
                    continue;
            }
            dx = (b->x - a->x) / 0x10000;
            dz = (b->z - a->z) / 0x10000;
            px = dx * dx;
            pz = dz * dz;
            px += pz;
            fp = Func_8000948;
            d = fp(px);
            if (*fl & 0x10)
                d = d * 2 / 3;
            if (d >= bestd)
                continue;
            ang = (unsigned short)atan2(b->z - a->z, b->x - a->x);
            lim = 0x1800;
            if (d > 0x13)
                lim = 0x1000;
            if (mode == 2)
                lim = 0x2000;
            if (d > 11) {
                diff = (short)(ang - dir);
                if (diff < 0)
                    diff = -diff;
                if (diff >= lim)
                    continue;
            }
            best = i;
            bestd = d;
        }
    }
    return best;
}
