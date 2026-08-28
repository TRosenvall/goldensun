struct Actor {
    unsigned char pad00[8];
    int x;
    int fc;
    int z;
    unsigned char pad14[0x14];
    int f28;
    unsigned char pad2c[0x1c];
    int f48;
};

extern struct Actor *__MapActor_GetActor(int slot);

int OvlFunc_935_2008944(int slot)
{
    struct Actor *ref;
    struct Actor *a;
    int i, d, x0, z0, x1, z1, dz;

    ref = __MapActor_GetActor(slot);
    for (i = 0; i <= 3; i++) {
        a = __MapActor_GetActor(i + 0xb);
        d = a->fc;
        if (d > 0 && d < 0x100000) {
            z1 = a->z / 0x100000;
            x1 = a->x / 0x100000;
            z0 = ref->z / 0x100000;
            x0 = ref->x / 0x100000;
            dz = z0 - z1;
            if (x0 == x1 && dz == 0) {
                a->fc = 0xff << 16;
                a->f48 = dz;
                a->f28 = dz;
                return 1;
            }
        }
    }
    return 0;
}
