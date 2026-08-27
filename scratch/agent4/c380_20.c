struct R { unsigned char pad00[4]; int f4; int f8; int fc; int f10; };
struct S { int x; int y; struct R r; };
extern void OvlFunc_common2_618(struct S *in, struct R *r);
extern int OvlFunc_common2_40c(struct R *r);
extern int OvlFunc_common2_3ec(struct R *r);
extern int OvlFunc_common2_3fc(struct R *r);
extern int OvlFunc_common2_41c(int a, int b, int c);
int OvlFunc_common2_380(int a, int b)
{
    struct S s;
    struct S *q;
    int t;
    int v;

    q = &s;
    q->x = a;
    q->y = b;
    OvlFunc_common2_618(q, &q->r);
    if (OvlFunc_common2_40c(&s.r)) return 0;
    if (OvlFunc_common2_3ec(&s.r)) return 0;
    if (OvlFunc_common2_3fc(&s.r) == 0) {
        t = s.r.f8;
        if (t < 0) return 0;
        if (t <= 0x1e) goto calc;
    }
    return (s.r.f4 != 0) + 0x7fffffff;
calc:
    v = OvlFunc_common2_41c(s.r.fc, s.r.f10, 0x3c - t);
    if (s.r.f4 != 0) v = -v;
    return v;
}
