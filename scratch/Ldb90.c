extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200db90(unsigned char *a)
{
    unsigned short *p;
    unsigned char *q;
    int ang;
    int r;
    int c, s;
    int v;

    p = (unsigned short *)(a + 0x64);
    q = *(unsigned char **)(a + 0x68);
    ang = *p;
    c = __cos(ang);
    r = *(int *)(a + 0x30) + 0x1c;
    *(int *)(a + 8) = *(int *)(q + 8) + c * r;
    s = __sin(ang);
    *(int *)(a + 0x10) = (s << 4) + (0xa4 << 16);
    *(int *)(a + 0x38) = *(int *)(a + 8);
    *(int *)(a + 0x40) = *(int *)(a + 0x10);
    v = *p;
    v += 0xfffffe00;
    *p = v;
}
