/* OvlFunc_881_2008598  --  0x02008598, was
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_a_a.s (this function alone), so it
 * converts whole. Matched from scratch. Declaring the spilled `r` after xmin moved
 * its slot to sp+0: declaration order sets pseudo order, and so spill-slot order.
 */
struct Info {
    unsigned char pad00[0x18];
    int f18;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x20 - 0x14];
    unsigned short f20;
    unsigned char pad22[0x50 - 0x22];
    struct Info *f50;
    unsigned char f54;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char gDebugMode;
extern struct Actor *__MapActor_GetActor(int id);
extern struct Actor *__GetFieldActor(int id);
extern int __GetFlag(int id);

void OvlFunc_881_2008598(void)
{
    unsigned char *g;
    struct Actor *a;
    struct Actor *f;
    unsigned char *blk;
    int x;
    int z;
    int xmin;
    int xmax;
    int zmin;
    int zmax;
    int r;
    unsigned int i;
    int fx;
    int fz;
    int rad;
    int dx;
    int lim;
    int w;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    r = a->f50->f18 * a->f20;
    blk = iwram_3001ebc;
    f = *(struct Actor **)(blk + 0x1e0);
    x = f->f8;
    xmin = x - 0xa00000;
    xmax = x + 0xa00000;
    z = f->f10;
    zmin = z - 0x12c0000;
    zmax = z + 0xc80000;
    for (i = 8; i <= 0x41; i++) {
        f = __GetFieldActor(i);
        if (f == 0)
            continue;
        fx = f->f8;
        fz = f->f10;
        if (fx < xmin || fx > xmax || fz < zmin || fz > zmax) {
            f->f54 = 0;
            continue;
        }
        f->f54 = 1;
        if (gDebugMode && __GetFlag(0x163))
            continue;
        rad = f->f50->f18;
        dx = f->f8 - a->f8;
        if (dx < 0)
            dx = a->f8 - f->f8;
        lim = r + f->f20 * rad;
        if (dx + (f->f10 - a->f10 >= 0 ? f->f10 - a->f10 : a->f10 - f->f10) < lim
            && !__GetFlag(0x104)) {
            w = i + 100;
            *(unsigned short *)(blk + 0x16c) = w;
        }
    }
}
