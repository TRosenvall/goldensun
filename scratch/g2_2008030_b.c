struct A {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    short f64;
    short f66;
};

extern unsigned int __Random(void);
extern unsigned int _umodsi3_RAM(unsigned int, unsigned int);

int OvlFunc_921_2008030(struct A *a)
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
        a->f18 += 0x80 << 5;
        a->f1c += -0x800;
        if (a->f66 != 0)
            a->f64 = _umodsi3_RAM(__Random(), 0x28) + 0x28;
        else
            a->f64 = _umodsi3_RAM(__Random(), 0x14) + 0x14;
        break;
    }
    a->f64--;
    return 1;
}
