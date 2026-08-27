struct S {
    unsigned char pad0[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
    unsigned char pada[0x15 - 0xa];
    unsigned char glo : 2;
    unsigned char gsel : 2;
    unsigned char ghi : 4;
};

struct A {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct S *f50;
};

int OvlFunc_947_2009938(struct A *a, struct A *b)
{
    unsigned char *p;
    unsigned int as;
    unsigned int bs;
    int r;

    r = 0;
    if (b->f8 == a->f8 && b->fc == a->fc && b->f10 == a->f10) {
        goto out;
    }
    if (b->f8 - 0x100000 >= a->f8) {
        goto out;
    }
    if (a->f8 >= b->f8 + 0x100000) {
        goto out;
    }
    if (b->fc / 0x10000 != a->fc / 0x10000) {
        goto out;
    }
    if (b->f10 <= a->f10) {
        goto out;
    }
    if (b->f10 - 0x200000 >= a->f10) {
        goto out;
    }
    as = a->f50->sel;
    bs = b->f50->sel;
    if (as < bs) {
        p = &a->f23;
        *p &= 0xfe;
        a->f50->sel = bs;
        a->f50->gsel = b->f50->gsel;
    }
    r = 1;
out:
    return r;
}
