extern unsigned char *iwram_3001ebc;
extern void _DeleteActor(void *e);

void Func_808b8e8(void)
{
    unsigned char *p;
    unsigned char *pl;
    void **slot;
    unsigned char *e;
    unsigned char *q;
    int xmin, xmax, zmin, zmax;
    int mask;
    int i;
    int x, z;

    p = iwram_3001ebc;
    pl = *(unsigned char **)(p + 0x1e0);
    xmin = *(int *)(pl + 8) - 0xa00000;
    xmax = *(int *)(pl + 8) + 0xa00000;
    zmin = *(int *)(pl + 0x10) - 0xc80000;
    zmax = *(int *)(pl + 0x10) + 0x640000;
    mask = -2;
    slot = (void **)(p + 0x34);
    for (i = 0; i < 0x3a; i++) {
        e = (unsigned char *)*slot;
        if (e != 0) {
            x = *(int *)(e + 8);
            z = *(int *)(e + 0x10);
            if (x != 0 || z != 0) {
                if (x < xmin || x > xmax || z < zmin || z > zmax) {
                    q = e + 0x54;
                    *q = 1;
                    (*(unsigned char **)(e + 0x50))[0x1d] &= mask;
                    _DeleteActor(e);
                    *slot = 0;
                }
            }
        }
        slot++;
    }
}
