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
    int *vp;
    struct W *wp;
    int r;

    vp = v;
    wp = &w;
    vp[1] = b;
    vp[0] = a;
    OvlFunc_common2_618(vp, wp);
    if (OvlFunc_common2_40c(wp) != 0 || OvlFunc_common2_3ec(wp) != 0)
        return 0;
    if (OvlFunc_common2_3fc(wp) != 0)
        goto big;
    if (wp->f8 < 0)
        return 0;
    if (wp->f8 > 0x1e) {
big:
        return 0x7fffffff + (wp->f4 != 0);
    }
    r = OvlFunc_common2_41c(wp->fc, wp->f10, 0x3c - wp->f8);
    if (wp->f4 != 0)
        r = -r;
    return r;
}
