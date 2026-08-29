struct Actor {
    unsigned char pad00[0x24];
    int f24;
    unsigned char pad28[4];
    int f2c;
    int f30;
    int f34;
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern unsigned char gState[];
extern struct Actor *GetFieldActor(int id);
extern void _Actor_SetAnim(struct Actor *a, int anim);

void Func_8091660(void)
{
    unsigned char *g;
    struct Actor *a;

    g = gState;
    a = GetFieldActor(*(int *)(g + 0x1f4));
    a->f30 = 0x10000;
    a->f34 = 0x8000;
    a->f38 = 0x80000000;
    a->f40 = 0x80000000;
    a->f24 = 0;
    a->f2c = 0;
    if (g[0x1f2] == 1)
        _Actor_SetAnim(a, 0xc);
    else
        _Actor_SetAnim(a, 1);
}
