extern int *iwram_3001f30;
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern int Func_809ba34(char *e);
extern void Func_809bb34(char *e);

void Func_8098b10(char *e)
{
    int *g;
    char *p;
    int v[3];
    int *o;
    int h;
    int k;

    g = iwram_3001f30;
    p = e + 0x40;
top:
    k = *(signed char *)p;
    if (k == 0) {
        v[0] = *(int *)(e + 0x14);
        v[2] = *(int *)(e + 0x18);
        vec3_translate(0xc8 << 13, (unsigned short)Random(), v);
        *(int *)(e + 0xc) = v[0];
        *(int *)(e + 0x10) = v[2];
        *(int *)(e + 0x24) = 0xc0 << 10;
        *(int *)(e + 0x20) = 0xc0 << 10;
        *(char *)(e + 0x42) = k;
        goto join;
    }
    if (k == 1) {
        register char *q __asm__("r2");
        if (Func_809ba34(e) != 0)
            return;
        q = p;
        *q = *q + 1;
        goto top;
    }
    if (k == 2) {
        o = (int *)g[4];
        v[0] = o[2];
        v[1] = o[3] + (0x80 << 13);
        v[2] = o[4];
        vec3_translate(0x80 << 12, g[0], v);
        Func_80974d8(v);
        vec3_translate(0x80 << 11, Random(), v);
        *(int *)(e + 0xc) = v[0];
        *(int *)(e + 0x10) = v[2];
        h = 0x80 << 4;
        *(short *)(e + 0x32) = h;
        *(char *)(e + 0x42) = 1;
join:
        *p = *p + 1;
        return;
    }
    if (k == 3) {
        if (Func_809ba34(e) == 0)
            Func_809bb34(e);
    }
}
