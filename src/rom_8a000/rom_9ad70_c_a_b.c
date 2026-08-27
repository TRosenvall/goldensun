struct S {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[4];
    int fc;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x34];
    unsigned char f54;
};

void Func_809b0dc(struct S *p)
{
    int d;
    int v;

    d = -0x280;
    p->f1c += d;
    v = p->f18 + d;
    p->f6 += 0x80 << 6;
    p->fc += 0x80 << 9;
    p->f18 = v;
    if (v < 0xc0 << 6)
        p->f54 = 0;
}
