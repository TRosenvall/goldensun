extern int *iwram_3001f30;
extern unsigned char Data_9f0b0[];
extern void vec3_translate(int a, int b, int *v);
extern void _Actor_SetScript(unsigned char *e, unsigned char *s);

void Func_8099340(unsigned char *e)
{
    int v[3];
    int *o;
    short *c;
    int n;

    o = iwram_3001f30;
    if (e == 0)
        return;
    c = (short *)(e + 0x64);
    n = --(*c);
    if (n != 0) {
        v[0] = o[1];
        v[1] = o[2] + (0xa0 << 12);
        v[2] = o[3];
        vec3_translate(n << 16, *(short *)(e + 0x66) + (n << 11), v);
        *(int *)(e + 8) = v[0];
        *(int *)(e + 0xc) = v[1];
        *(int *)(e + 0x10) = v[2];
    } else {
        _Actor_SetScript(e, Data_9f0b0);
    }
}
