struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *__Func_8093554(void);

int OvlFunc_959_2009980(int slot)
{
    struct Actor *a;
    struct Actor *b;
    int ax;
    int az;
    int bx;
    int bz;
    int dx;
    int dz;

    a = __MapActor_GetActor(slot);
    b = __Func_8093554();
    ax = a->x / 0x100000;
    az = a->z / 0x100000;
    bx = b->x / 0x100000;
    bz = b->z / 0x100000;
    dx = ax - bx;
    if (dx < 0)
        dx = -dx;
    dz = az - bz;
    if (dz < 0)
        dz = -dz;
    if (dx > 7 || dz > 5)
        return 0;
    return 1;
}
