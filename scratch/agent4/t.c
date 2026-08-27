typedef struct Obj {
    int u00;
    int u04;
    int x08;
    int x0c;
    int x10;
    int u14[7];
    int x30;
    int x34;
    int x38;
    int x3c;
    int x40;
    int u44[8];
    short s64;
} Obj;

void OvlFunc_898_2009754(Obj *p)
{
    int c;
    int d;
    int v;
    int w;

    p->x08 += p->x30;
    p->x38 = p->x08;
    if (p->s64 != 0) {
        c = p->x0c;
        d = p->x34;
    } else {
        p->x10 += p->x34;
        p->x40 = p->x10;
        d = 0x400;
        c = p->x0c;
    }
    p->x0c = c + d;
    p->x3c = p->x0c;
    v = p->x30;
    v -= v / 28;
    p->x30 = v;
    w = p->x34;
    w -= w / 28;
    p->x34 = w;
}
