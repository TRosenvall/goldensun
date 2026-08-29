struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct Actor *__MapActor_GetActor(int slot);

int OvlFunc_959_2009918(int slot)
{
    struct Actor *a;
    struct Actor *b;
    int az;
    int ax;
    int bz;
    int bx;
    int dx;
    int dz;

    a = __MapActor_GetActor(slot);
    b = __MapActor_GetActor(0);
    az = a->z / 0x100000;
    ax = a->x / 0x100000;
    bz = b->z / 0x100000;
    bx = b->x / 0x100000;
    dx = ax - bx;
    az++;
    if (dx < 0)
        dx = -dx;
    dz = az - bz;
    if (dz < 0)
        dz = -dz;
    if (dx + dz <= 4)
        return 1;
    return 0;
}
