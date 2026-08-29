struct A { unsigned char pad00[6]; unsigned short f6; };

extern unsigned char gState[];
extern void Func_809b450(struct A *a);

void Func_809b5dc(struct A *a)
{
    short *p64;
    short *p66;
    int v64;
    int h;
    unsigned char *g;
    int area;

    p64 = (short *)((char *)a + 0x64);
    v64 = *p64;
    p66 = (short *)((char *)a + 0x66);
    h = *(unsigned short *)p66;
    *(unsigned short *)p66 = h + 1;
    g = gState;
    area = *(short *)(g + (0xed << 1));
    if (area == 1) {
        if ((short)h % 7 == 0)
            Func_809b450(a);
    } else {
        if ((short)h % 5 == 0)
            Func_809b450(a);
    }
    if (v64 == 1)
        a->f6 = a->f6 + (0xc0 << 4);
}
