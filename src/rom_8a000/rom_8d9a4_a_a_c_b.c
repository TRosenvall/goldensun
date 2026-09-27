/* Func_808ddec  --  0x0808ddec, split out of asm/rom_8a000/rom_8d9a4_a_a_c.s;
 * Func_808df1c stays in _c.s. Matched from scratch.
 *
 * - gcc rewrites `x < -0x2fff || x > 0x2fff` to <= -0x3000 / >= 0x3000 (0x3000
 *   is an ARM immediate, 0x2fff is not) and folds the pair into one unsigned
 *   range test. Each limit in a local assigned just before its own test keeps
 *   the ROM's `ldr =0x2fff / bgt` shape.
 * - Reload registers are handed out in ROTATION, so drift that looks like a
 *   prologue problem is often a different reload elsewhere: each square in its
 *   own temporary, then `px += py; px += pz`, fixed it. Declaration order was
 *   inert across 192 permutations.
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
extern int Func_8000948(int v);
extern int atan2(int dz, int dx);

int Func_808ddec(int self)
{
    struct Ent *a;
    struct Ent *b;
    unsigned char *fl;
    int best = -1;
    int bestd = 0x20;
    int i;
    int dx, dy, dz, d;
    int px, py, pz;
    int ang, diff;
    int (*fp)(int);

    a = GetFieldActor(self);
    if (a != 0) {
        for (i = 0; i <= 0x42; i++) {
            if (i == self)
                continue;
            b = GetFieldActor(i);
            if (b == 0)
                continue;
            fl = &b->f59;
            if (*fl & 8)
                continue;
            if (b->y - a->y >= 0) {
                if (b->y - a->y > 0x2fffff)
                    continue;
            } else {
                if (a->y - b->y > 0x2fffff)
                    continue;
            }
            dx = (b->x - a->x) / 0x10000;
            dy = (b->y - a->y) / 0x10000;
            dz = (b->z - a->z) / 0x10000;
            px = dx * dx;
            py = dy * dy;
            pz = dz * dz;
            px += py;
            px += pz;
            fp = Func_8000948;
            d = fp(px);
            if (*fl & 4)
                d = d * 10 / 13;
            if (d >= bestd)
                continue;
            ang = (unsigned short)atan2(b->z - a->z, b->x - a->x);
            if (d > 11) {
                int lo, hi;
                diff = (short)(ang - a->f06);
                lo = -0x2fff;
                if (diff < lo)
                    continue;
                hi = 0x2fff;
                if (diff > hi)
                    continue;
            }
            best = i;
            bestd = d;
        }
    }
    return best;
}
