extern void _Actor_TravelTo(void *a, int x, int y, int z);
extern void _Actor_SetAnim(void *a, int anim);
extern int Func_8000948(int v);

int Func_809397c(void *ent)
{
    unsigned char *e;
    unsigned char *t;
    int dx, dz, d, thr, s;
    int (*fp)(int);

    e = (unsigned char *)ent;
    t = *(unsigned char **)(e + 0x68);
    if (t != 0) {
        dx = (*(int *)(t + 8) - *(int *)(e + 8)) / 0x10000;
        dz = (*(int *)(t + 0x10) - *(int *)(e + 0x10)) / 0x10000;
        s = dz * dz;
        s += dx * dx;
        fp = Func_8000948;
        d = fp(s);
        thr = *(short *)(e + 0x64);
        if (d >= thr) {
            int nx = *(int *)(e + 8) + ((dx << 20) / thr);
            int nz = *(int *)(e + 0x10) + ((dz << 20) / thr);
            _Actor_TravelTo(e, nx, *(int *)(e + 0xc), nz);
            _Actor_SetAnim(e, 2);
        } else {
            _Actor_SetAnim(e, 1);
        }
    }
    return 1;
}
