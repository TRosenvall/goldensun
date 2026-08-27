struct S {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x66 - 0x44];
    short f66;
};

void OvlFunc_905_2008a00(struct S *s)
{
    switch (s->f66) {
    case 0:
        s->f8 = s->f8 + s->f30;
        s->f38 = s->f8;
        s->fc = s->fc + s->f34;
        s->f3c = s->fc;
        break;
    case 1:
        s->f8 = s->f8 + s->f30;
        s->f38 = s->f8;
        s->f10 = s->f10 + s->f34;
        s->f40 = s->f10;
        break;
    case 2:
        s->fc = s->fc + s->f30;
        s->f3c = s->fc;
        s->f10 = s->f10 + s->f34;
        s->f40 = s->f10;
        break;
    }
}
