struct Model { unsigned char pad00[0x28]; short *f28; };

struct Ent {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    unsigned char pad0c[4];
    int z;
    unsigned char pad14[0x3c];
    struct Model *f50;
};

struct Rect { int x0, z0, x1, z1; };

extern unsigned char iwram_3001ebc[];
extern int L6190[];
extern int L61d0[];
extern struct Rect L61e8[];
extern struct Ent *__MapActor_GetActor(int slot);

struct Ent *OvlFunc_883_200834c(int *facingOut, int *slotOut, int *modelOut)
{
    struct Ent **tbl;
    struct Ent *pl;
    struct Ent *e;
    unsigned int slot, i;
    int s, tx, tz, x0, z0, x1, z1, ex, ez, id;

    tbl = (struct Ent **)(*(char **)iwram_3001ebc + 0x14);
    pl = __MapActor_GetActor(0);
    *facingOut = pl->facing >> 12;
    for (slot = 8; slot <= 0x41; slot++) {
        e = tbl[slot];
        id = *e->f50->f28;
        for (i = 0; i <= 5; i++) {
            if (id != L61d0[i])
                continue;
            *modelOut = i;
            s = L6190[*facingOut] >> 16;
            tx = ((pl->x >> 16) + s) >> 4;
            s = (short)L6190[*facingOut];
            tz = ((pl->z >> 16) + s) >> 4;
            ex = *(short *)((char *)e + 0xa);
            x0 = (ex + L61e8[i].x0) >> 4;
            ez = *(short *)((char *)e + 0x12);
            z0 = (ez + L61e8[i].z0) >> 4;
            x1 = (ex + L61e8[i].x1) >> 4;
            z1 = (ez + L61e8[i].z1) >> 4;
            if (x0 > tx)
                continue;
            if (tx >= x1)
                continue;
            if (z0 > tz)
                continue;
            if (tz >= z1)
                continue;
            if (i & 1) {
                if (x0 == (pl->x >> 20))
                    continue;
                *slotOut = slot;
                return e;
            } else {
                if (z0 == (pl->z >> 20))
                    continue;
                *slotOut = slot;
                return e;
            }
        }
    }
    return 0;
}
