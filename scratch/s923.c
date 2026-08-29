struct Emit {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18];
};

struct Src {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
};

extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, struct Emit *h);

int OvlFunc_923_2008cc0(struct Src *p)
{
    struct Emit s;
    unsigned int u;
    int k;

    s.f4 = 7;
    if ((iwram_3001e40 & 1) == 0)
        s.f4 = 5;
    s.f8 = 0xcccc;
    s.fc = 0xcccc;
    s.f0 = 0;
    u = (__Random() << 3) >> 16;
    k = u * 0x3333;
    OvlFunc_common0_10c(p->x + ((8 - (iwram_3001e40 & 0xf)) << 16),
                        p->y + (0xd0 << 13),
                        p->z,
                        0,
                        -k,
                        0,
                        0xb0 << 12,
                        &s);
    return 0;
}
