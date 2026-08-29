extern unsigned int __Random(void);

struct A {
    unsigned char pad00[6];
    short f06;
};

int OvlFunc_943_20088e0(struct A *a)
{
    unsigned char *p;
    unsigned int c;
    unsigned int v;

    p = (unsigned char *)a + 0x62;
    c = *p;
    if (c != 0) {
        *p = c + 0xff;
    } else {
        v = (__Random() * 300) >> 16;
        if (v > 0xc8)
            a->f06 = 0xd0 << 8;
        else if (v > 0x64)
            a->f06 = 0xa0 << 7;
        else
            a->f06 = c;
        *p = ((__Random() * 80) >> 16) + 0x50;
    }
    return 1;
}
