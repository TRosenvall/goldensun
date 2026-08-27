struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[4];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern struct A *__MapActor_GetActor(int slot);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200b600(struct A *a)
{
    struct A *b;
    unsigned short *p;
    int ang;

    b = __MapActor_GetActor(0x18);
    p = (unsigned short *)((char *)a + 0x64);
    ang = *p;
    a->f8 = b->f8 + __cos(ang) * (a->f30 + 3);
    a->f10 = b->f10 + (__sin(ang) << 1);
    a->f40 = a->f10;
    a->f38 = a->f8;
    *p = *p - 0x800;
}
