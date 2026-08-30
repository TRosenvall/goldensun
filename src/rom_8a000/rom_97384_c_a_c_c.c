extern unsigned char *iwram_3001ea8;
extern void Func_8097948(int ang, int *a, int *b, int *c);

void Func_80978c4(void)
{
    unsigned char *b;
    unsigned char *p;
    unsigned char *q0, *q1, *q2;
    int z;
    int off;
    int a0, a1, a2;

    b = iwram_3001ea8;
    p = b + 0x28e;
    z = 0;
    a0 = 0;
    a1 = 0;
    a2 = 0;
    Func_8097948(*(unsigned short *)p << 16, &a0, &a1, &a2);
    off = 0x28b;
    q0 = b + off;
    *q0 = (a0 >> 18) + 4;
    off += 1;
    q1 = b + off;
    *q1 = (a1 >> 18) + 4;
    off += 1;
    q2 = b + off;
    *q2 = (a2 >> 18) + 4;
    *(unsigned short *)p += 4;
    *q0 &= 0x1f;
    *q1 &= 0x1f;
    *q2 &= 0x1f;
    if (*(unsigned short *)p >= 360)
        *(unsigned short *)p = z;
}
