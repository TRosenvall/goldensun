struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };
extern struct Actor *__MapActor_GetActor(int id);

int OvlFunc_959_2009880(int id)
{
    struct Actor *m;
    struct Actor *p;
    int mz, mx, pz, px;

    m = __MapActor_GetActor(id);
    p = __MapActor_GetActor(0);
    mz = m->f10 / 0x100000;
    mx = m->f8 / 0x100000;
    pz = p->f10 / 0x100000;
    px = p->f8 / 0x100000;
    if (mx - px < -6 || mx - px > 6)
        return 0;
    if (mz - 2 < pz && mz + 2 > pz)
        return 1;
    return 0;
}
