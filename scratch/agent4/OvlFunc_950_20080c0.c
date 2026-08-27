struct Actor {
    unsigned char pad00[0x55];
    unsigned char f55;
};

struct Warp {
    int dest;
    unsigned short x;
    unsigned short z;
};

extern unsigned char *iwram_3001ebc;
extern struct Warp warps[] __asm__(".L1dcc");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern int __Func_8010560();
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8091e9c(int a);
void OvlFunc_950_20080c0(void)
{
    struct Actor *a;
    struct Actor *b;
    short *p;
    unsigned int i;
    int k;
    int d;
    unsigned short x;
    unsigned short z;

    p = (short *)iwram_3001ebc;
    for (i = 8; i <= 0x41; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0)
            a->f55 = 0;
    }
    p = (short *)((unsigned char *)p + 0x16c);
    k = *p - 0xe;
    __PlaySound(0x9e);
    z = warps[k].z;
    x = warps[k].x;
    d = warps[k].dest;
    __Func_8010560(d, x, z);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    b = __MapActor_GetActor(0);
    b->f55 = 0;
    __MapActor_SetAnim(0, 2);
    __Func_8091e9c(*p);
}
