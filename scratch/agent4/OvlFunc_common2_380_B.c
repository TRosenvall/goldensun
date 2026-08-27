struct R { unsigned char pad00[4]; int f4; int f8; int fc; int f10; };
extern void OvlFunc_common2_618(int *in, struct R *r);
extern int OvlFunc_common2_40c(struct R *r);
extern int OvlFunc_common2_3ec(struct R *r);
extern int OvlFunc_common2_3fc(struct R *r);
extern int OvlFunc_common2_41c(int a, int b, int c);
int OvlFunc_common2_380(int a, int b)
{
    int in[2];
    struct R r;
    int t;
    int v;
    int *q;

    q = in;
    *q++ = a;
    *q = b;
    q = in;
    OvlFunc_common2_618(q, &r);
    if (OvlFunc_common2_40c(&r)) return 0;
    if (OvlFunc_common2_3ec(&r)) return 0;
    if (OvlFunc_common2_3fc(&r) == 0) {
        t = r.f8;
        if (t < 0) return 0;
        if (t <= 0x1e) goto calc;
    }
    return (r.f4 != 0) + 0x7fffffff;
calc:
    v = OvlFunc_common2_41c(r.fc, r.f10, 0x3c - t);
    if (r.f4 != 0) v = -v;
    return v;
}
