struct Model { unsigned char pad00[0x28]; short *f28; };
struct Pos { int x, y, z; };
struct Ent { unsigned char pad00[8]; struct Pos p; unsigned char pad14[0x3c]; struct Model *f50; };
struct Rect { int x0, z0, x1, z1; };
struct Work { unsigned int idx; int pad04; struct Pos p; int pad14; };
extern unsigned char *iwram_3001e70;
extern int L2684[] __asm__(".L2684");
extern struct Rect L269c[] __asm__(".L269c");
extern struct Ent *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_948_2008244(int a, int b, int c, int d, int e, int f);

int OvlFunc_948_20088c0(int slot)
{
    unsigned char *cam;
    struct Ent *e;
    struct Work w;
    unsigned int i;
    int xe, ze, ex, ez;
    int cz;
    cam = iwram_3001e70;
    e = __MapActor_GetActor(slot);
    for (i = 0; i <= 5; i++) {
        if (*e->f50->f28 == L2684[i]) {
            w.idx = i;
            break;
        }
        w.idx = 7;
    }
    if (w.idx > 6)
        return 0;
    w.p.x = ex = e->p.x;
    w.p.y = e->p.y;
    w.p.z = ez = e->p.z;
    ze = (((L269c[w.idx].z0 < 0) ? -L269c[w.idx].z0 : L269c[w.idx].z0) + ((L269c[w.idx].z1 < 0) ? -L269c[w.idx].z1 : L269c[w.idx].z1)) >> 4;
    xe = (((L269c[w.idx].x0 < 0) ? -L269c[w.idx].x0 : L269c[w.idx].x0) + ((L269c[w.idx].x1 < 0) ? -L269c[w.idx].x1 : L269c[w.idx].x1)) >> 4;
    w.p.x = ex + (L269c[w.idx].x0 << 16);
    w.p.z = ez + (L269c[w.idx].z0 << 16);
    w.p.x >>= 20;
    w.p.z >>= 20;
    i = *(int *)(cam + (0x9e << 1)) >> 20;
    cz = *(int *)(cam + (0xa0 << 1)) >> 20;
    __Func_8010704(w.p.x, w.p.z, xe, ze, i + w.p.x, cz + w.p.z);
    OvlFunc_948_2008244(0, w.p.x, w.p.z, xe, ze, 0xff);
    OvlFunc_948_2008244(2, w.p.x, w.p.z, xe, ze, 0xff);
    return 1;
}
