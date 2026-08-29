extern unsigned int __Random(void);

struct A {
    unsigned char pad00[0xc];
    int f0c;
};

int OvlFunc_943_2008514(struct A *a)
{
    short *d;
    unsigned int v;

    d = (short *)((char *)a + 0x66);
    if (*d != 0) {
        v = (__Random() << 15) >> 16;
        a->f0c = a->f0c - v - 0x8000;
        if (a->f0c < (0x80 << 11)) {
            int z = 0;
            *d = z;
        }
    } else {
        v = (__Random() << 15) >> 16;
        a->f0c = a->f0c + v + (0x80 << 8);
        if (a->f0c > (0xc0 << 12)) {
            int o = 1;
            *d = o;
        }
    }
    return 1;
}
