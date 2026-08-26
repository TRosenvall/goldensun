struct B { unsigned char pad00[8]; int x; unsigned char pad0c[4]; int z; };

extern struct B **GetBattleActor(int id);
extern void Func_80c0cec(int x, int y, int z, int w);

void Func_80c0df4(int idA, int idB, int c)
{
    struct B *a;
    struct B *b;
    int sx, sz, ax, bx, az, bz;

    a = *GetBattleActor(idA);
    b = *GetBattleActor(idB);
    sx = b->x + a->x;
    sz = b->z + a->z;
    Func_80c0cec(sx / 2, 0, sz / 2, c);
}
