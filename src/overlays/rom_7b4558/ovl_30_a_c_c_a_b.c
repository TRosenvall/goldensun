struct P {
    int f0;
    unsigned char pad4[0x28 - 4];
};

struct V { int x; int y; int z; };

struct A { unsigned char pad0[8]; int f8; unsigned char padc[4]; int f10; };

extern struct A *__MapActor_GetActor(void);
extern void __PlaySound(int id);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);

void OvlFunc_927_2008e18(void)
{
    struct V v;
    struct P p;
    struct A *a;
    unsigned int i;
    int ang;

    a = __MapActor_GetActor();
    __PlaySound(0xbc);
    p.f0 = 1;
    for (i = 0; i <= 0x10; i++) {
        ang = i << 12;
        v.x = __cos(ang);
        v.y = 0;
        v.z = __sin(ang);
        v.x += v.x / 3;
        OvlFunc_927_2008ae8(a->f8, 0x80 << 13, a->f10, v.x,
                            v.y + 0x1999, v.z, 0x80 << 10, &p);
    }
}
