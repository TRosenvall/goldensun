extern unsigned char *iwram_3001f2c;
extern void _UpdateSprite(int a, int *b, int *c, int d);

void Func_80200cc(void)
{
    int a[2];
    int b[4];
    short i;
    unsigned char *base;
    int *p;
    short *q;
    int v;

    i = 0;
    base = iwram_3001f2c;
    q = (short *)(base + (0x8d << 2));
    p = (int *)(base + 0x224);
    while (i <= 3) {
        v = p[i];
        if (v != 0) {
            a[0] = 0x80 << 9;
            a[1] = 0x80 << 9;
            b[0] = q[i] << 16;
            b[1] = 0xfa << 17;
            b[2] = (q[i + 4] << 16) + (0xfa << 17);
            b[3] = 0;
            _UpdateSprite(v, b, a, 0x80 << 7);
        }
        i++;
    }
}
