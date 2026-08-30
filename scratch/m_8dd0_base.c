extern int _AREA_00;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);

void OvlFunc_951_2008dd0(int slot, int *src, int h, int v, int sp)
{
    unsigned char *e;
    unsigned char *q;
    unsigned char **list;
    unsigned char *r;
    int n;
    int m;
    int t;

    e = __MapActor_GetActor(slot);
    if (e != 0) {
        *(int *)(e + 8) = *src++;
        *(int *)(e + 0xc) = *src++;
        *(int *)(e + 0x10) = *src;
        *(short *)(e + 6) = h;
        e[0x55] = (int)&_AREA_00;
        (*(unsigned char **)(e + 0x50))[0x26] = (int)&_AREA_00;
        __Actor_SetAnimSpeed(e, sp);
    }
    q = *(unsigned char **)(e + 0x50);
    if (q[0x27] != 0) {
        m = 0xff;
        list = (unsigned char **)(q + 0x28);
        n = q[0x27];
        do {
            r = *list++;
            if (r[5] != v) {
                t = r[0x16] | m;
                r[5] = v;
                r[0x16] = t;
            }
            n--;
        } while (n != 0);
    }
}
