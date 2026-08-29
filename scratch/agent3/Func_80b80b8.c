struct A {
    unsigned char pad0[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x14];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0xc];
    int f44;
    int f48;
    unsigned char pad4c[0xe];
    unsigned char f5a;
};

#define REG_BLDCNT (*(volatile unsigned short *)0x04000050)
extern void _Actor_TravelTo(struct A *a, int x, int y, int z);
extern void _Actor_SetAnim(struct A *a, int anim);

void Func_80b80b8(struct A **pa, struct A **pb, int s)
{
    struct A *a, *b;
    int x, z;

    a = *pa;
    b = *pb;
    x = a->f8 + s * (b->f8 - a->f8) / 100;
    z = a->f10 + s * (b->f10 - a->f10) / 100;
    REG_BLDCNT = 0;
    a->f34 = 0x20000;
    a->f30 = 0x80000;
    a->f28 = 0x40000;
    a->f48 = 0xab85;
    a->f44 = 0;
    a->f5a = 1;
    _Actor_TravelTo(a, x, 0, z);
    _Actor_SetAnim(a, 2);
}
