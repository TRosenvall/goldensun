struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct Actor *__MapActor_GetActor(int slot);

int OvlFunc_959_200981c(int slot)
{
    struct Actor *a;
    struct Actor *b;
    int az;
    int ax;
    int bz;
    int bx;

    a = __MapActor_GetActor(slot);
    b = __MapActor_GetActor(0);
    az = a->z / 0x100000;
    ax = a->x / 0x100000;
    bz = b->z / 0x100000;
    bx = b->x / 0x100000;
    if (az - bz >= -6 && az - bz <= 6 && ax - 1 < bx && ax + 1 > bx)
        return 1;
    return 0;
}
