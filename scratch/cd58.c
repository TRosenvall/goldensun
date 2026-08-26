struct E {
    unsigned char pad00[6];
    unsigned short f6;
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct E *__MapActor_GetActor(int slot);
extern int __atan2(int dz, int dx);
extern void __Func_8092adc(int a, int b, int c);

int OvlFunc_923_2008d58(struct E *e)
{
    struct E *pl;

    pl = __MapActor_GetActor(0);
    if ((pl->z >> 19) <= 0x16) {
        e->f6 = __atan2(pl->z - e->z, pl->x - e->x);
    } else if (e->f6 != (0xc0 << 8)) {
        __Func_8092adc(3, 0xc0 << 8, 0);
    }
    return 0;
}
