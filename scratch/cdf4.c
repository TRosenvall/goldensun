struct B { unsigned char pad00[8]; int x; unsigned char pad0c[4]; int z; };

extern struct B **GetBattleActor(int id);
extern void Func_80c0cec(int x, int y, int z, int w);

void Func_80c0df4(int idA, int idB, int c)
{
    struct B *a;
    struct B *b;

    a = *GetBattleActor(idA);
    b = *GetBattleActor(idB);
    Func_80c0cec((b->x + a->x) / 2, 0, (b->z + a->z) / 2, c);
}
