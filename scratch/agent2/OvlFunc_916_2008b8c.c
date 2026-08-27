struct Region { short f0; short x0; short z0; short flag; int f8; };

struct Region *OvlFunc_916_2008b8c(struct Region *p, int x, int z)
{
    int x0, z0, x1, z1, f;

    while (p->f0 != -1) {
        x1 = p->x0;
        z1 = p->z0;
        f = p->flag;
        x0 = x1;
        z0 = z1;
        if (f == 0)
            x1 += 3;
        else
            z1 += 3;
        if (x >= x0 && x <= x1 && z >= z0 && z <= z1)
            return p;
        p++;
    }
    return 0;
}
