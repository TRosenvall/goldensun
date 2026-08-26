struct A {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    unsigned short f64;
};

extern int L4468[] __asm__(".L4468");

void OvlFunc_957_2008ee0(struct A *a)
{
    unsigned short *p;
    int v;

    p = &a->f64;
    v = L4468[((short)*p >> 2) & 3];
    a->f18 = v;
    a->f1c = v;
    *p = (unsigned short)(*p + 1) & 0xf;
}
