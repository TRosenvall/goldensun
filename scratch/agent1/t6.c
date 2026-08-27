extern unsigned char *iwram_3001f2c;
extern void _UpdateSprite(int a, int *b, int *c, int d);

void Func_80200cc(void)
{
    int a[2];
    int b[4];
    int i;
    int A;
    int B;
    unsigned char *base;
    int *p;
    short *q;
    int v;

    i = 0;
    A = 0x80 << 9;
    base = iwram_3001f2c;
    B = 0xfa << 17;
    q = (short *)(base + (0x8d << 2));
    p = (int *)(base + 0x224);
    while (i <= 3) {
        v = *p++;
        if (v != 0) {
            a[0] = A;
            a[1] = A;
            b[0] = q[i] << 16;
            b[1] = B;
            b[2] = (q[i + 4] << 16) + B;
            b[3] = 0;
            _UpdateSprite(v, b, a, 0x80 << 7);
        }
        i++;
    }
}
