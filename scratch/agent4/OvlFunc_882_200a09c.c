struct Ent {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned char pad06[0x10 - 6];
    int f10;
};

struct B {
    unsigned char pad00[0x25];
    unsigned char f25;
    unsigned char pad26;
    unsigned char f27;
    struct Ent *f28[1];
};

struct A {
    unsigned char pad00[0x50];
    struct B *f50;
    unsigned char f54;
};

extern unsigned int iwram_3001e40;
extern unsigned char tbl[] __asm__(".L48bc");

void OvlFunc_882_200a09c(struct A *a, int b)
{
    struct B *p;
    struct Ent **q;
    struct Ent *e;
    int val;
    int n;

    if ((a->f54 & 0xf) != 1)
        return;
    p = a->f50;
    val = b - 1;
    if (b == 0)
        val = tbl[(iwram_3001e40 >> 1) & 1];
    if (p->f27 != 0) {
        q = p->f28;
        n = p->f27;
        do {
            e = *q++;
            if (e != 0 && e->f10 != 0)
                e->f5 = val;
            n--;
        } while (n != 0);
    }
    p->f25 = 1;
}
