struct Region { short f0; short x0; short z0; short flag; int f8; };

extern unsigned char gBuffer[];

void OvlFunc_916_2008b3c(struct Region *p, int val)
{
    int x, z, f, i;
    unsigned char *q;

    while (p->f0 != -1) {
        x = p->x0;
        z = p->z0;
        f = p->flag;
        for (i = 3; i >= 0; i--) {
            q = &gBuffer[(x + (z << 7)) << 2];
            q[2] = val;
            if (f == 0)
                x++;
            else
                z++;
        }
        p++;
    }
}
