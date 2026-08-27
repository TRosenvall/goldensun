struct W { int f0; int f4; int f8; int fc; int f10; };

extern void OvlFunc_common2_618(int *v, struct W *w);
extern unsigned int OvlFunc_common2_40c(struct W *w);
extern unsigned int OvlFunc_common2_3ec(struct W *w);
extern unsigned int OvlFunc_common2_3fc(struct W *w);
extern int OvlFunc_common2_41c(int a, int b, int c);

int OvlFunc_common2_380(int a, int b)
{
    int v[2];
    struct W w;
    int r;

    v[0] = a;
    v[1] = b;
    OvlFunc_common2_618(v, &w);
    if (OvlFunc_common2_40c(&w) != 0 || OvlFunc_common2_3ec(&w) != 0)
        return 0;
    if (OvlFunc_common2_3fc(&w) != 0)
        goto big;
    if (w.f8 < 0)
        return 0;
    if (w.f8 > 0x1e) {
big:
        return 0x7fffffff + (w.f4 != 0);
    }
    r = OvlFunc_common2_41c(w.fc, w.f10, 0x3c - w.f8);
    if (w.f4 != 0)
        r = -r;
    return r;
}
