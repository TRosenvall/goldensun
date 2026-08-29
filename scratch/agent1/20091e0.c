struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
};

struct Cfg {
    int f00;
    int f04;
    unsigned char pad08[0x1c];
    void (*f24)(void);
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_964_2009068(void);
extern void OvlFunc_964_2008ae8(int x, int y, int z, int a, int b, int c, int d, struct Cfg *s);

void OvlFunc_964_20091e0(int slot)
{
    int v[3];
    struct Cfg s;
    struct Actor *a;
    unsigned int i;
    int ang;

    a = __MapActor_GetActor(slot);
    s.f00 = 1;
    s.f04 = 7;
    s.f24 = OvlFunc_964_2009068;
    for (i = 0; i <= 0x10; i += 2) {
        ang = i << 12;
        v[0] = __cos(ang);
        v[1] = 0;
        v[2] = __sin(ang);
        v[0] = v[0] + v[0] / 3;
        OvlFunc_964_2008ae8(a->x, a->y, a->z, v[0], v[1], v[2], 0x1030001, &s);
    }
}
