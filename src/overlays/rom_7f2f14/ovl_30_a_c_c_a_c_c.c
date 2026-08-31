typedef struct {
    int f0;
    int f1;
    int f2;
    int f3;
    int f4;
    int f5;
    short f6;
    short f7;
    int f8;
    int f9;
    int f10;
} V;

extern int iwram_3001e40;
extern int __Random(void);
extern void __PlaySound(int id);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, void *h);

int OvlFunc_968_2008b98(int *o)
{
    V v;
    int e1;
    int z;
    int r;
    int t;

    e1 = 0x8f << 1;
    if ((iwram_3001e40 & 3) != 0)
        return 0;
    if (((unsigned int)(__Random() * 6) >> 16) == 0) {
        if (o[0xe] != (0x80 << 24) || o[0x10] != o[0xe])
            __PlaySound(0xf6);
    }
    z = 0;
    v.f6 = e1;
    v.f2 = 0x80 << 9;
    v.f3 = 0x80 << 9;
    v.f4 = 0xfffffeb9;
    v.f5 = 0xfffffeb9;
    r = ((int)(((unsigned int)(__Random() * 9) >> 16) - 4) << 16) / 0xa;
    t = ((int)(((unsigned int)(__Random() * 9) >> 16) - 4) << 16) / 0xa;
    OvlFunc_968_2008118(o[2], o[3], o[4] + 0xffff0000, r, z, t, 0x1c0001, &v);
    return 0;
}
