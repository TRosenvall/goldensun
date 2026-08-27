struct R {
    unsigned char pad00[8];
    unsigned short f8;
    unsigned short fa;
    unsigned short fc;
    unsigned short fe;
    unsigned char pad10[2];
    unsigned short f12;
};

struct Box {
    struct R *f0;
    unsigned char pad04[0x14 - 4];
    unsigned short f14;
};

extern void Func_80170f8(int a, int b, int c, int d);
extern void ClearUIRegion(int a, int b, int c, int d);

void Func_8019854(struct Box *b)
{
    struct R *r;
    int x;
    int y;
    int w;
    int h;
    int m;

    r = b->f0;
    m = r->f12;
    x = r->fc;
    y = r->fe;
    w = r->f8;
    h = r->fa;
    if (m == 4) {
        Func_80170f8(x - 1, y - 1, w + 2, h + 2);
        b->f14 += 0xffff;
        if (b->f14 == 0) {
            b->f0->f12 = 0;
            ClearUIRegion(x - 1, y - 1, w + 2, h + 2);
            Func_80170f8(x, y, w, h);
        }
    }
}
