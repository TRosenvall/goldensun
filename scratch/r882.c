struct A {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    short f64;
};

extern unsigned int __Random(void);

int OvlFunc_882_2008064(struct A *a)
{
    switch (a->f64) {
    case 6:
        a->f18 += -0x4000;
        a->f1c += 0x80 << 6;
        break;
    case 4:
        a->f18 += 0x80 << 6;
        a->f1c += -0x1000;
        break;
    case 2:
        a->f18 += 0x80 << 5;
        a->f1c += -0x800;
        break;
    case 0:
        a->f18 = 0x80 << 9;
        a->f1c = 0x80 << 9;
        a->f64 = __Random() % 0x5a + 0x3c;
        break;
    }
    a->f64--;
    return 1;
}
