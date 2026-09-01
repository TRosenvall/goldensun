extern void vec3_translate(int a, int b, int *v);
extern void Func_8099040(void);

void Func_80990cc(unsigned char *e)
{
    int v[3];
    int *p;
    short *c;
    int n;

    if (e == 0)
        return;
    c = (short *)(e + 0x64);
    n = --(*c);
    if (n != 0) {
        p = v;
        p[0] = *(int *)(e + 0x38);
        p[1] = *(int *)(e + 0x3c);
        p[2] = *(int *)(e + 0x40);
        vec3_translate(n << 17, *(short *)(e + 0x66) - (n << 11), p);
        *(int *)(e + 8) = p[0];
        *(int *)(e + 0xc) = p[1];
        *(int *)(e + 0x10) = p[2];
    } else {
        *(void **)(e + 0x6c) = Func_8099040;
    }
}
