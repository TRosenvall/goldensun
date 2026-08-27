struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
};

extern unsigned char gScript_970__020094c4[];
extern unsigned int __Random(void);
extern void __Actor_SetAnim(struct A *a, int n);
extern void __Actor_SetScript(struct A *a, unsigned char *s);

void OvlFunc_970_2008100(struct A *a)
{
    short *p;
    short *q;
    int v;
    int h;
    int t;
    int r;

    p = (short *)((char *)a + 0x64);
    v = *p;
    h = *(unsigned short *)p;
    if (v != 0) {
        *(unsigned short *)p = h - 1;
        r = __Random();
        a->f8 += r - __Random();
        a->fc += 0xcccc;
    } else {
        q = (short *)((char *)a + 0x66);
        if (*q != 0) {
            *q = v;
            __Actor_SetAnim(a, 1);
            t = 0x14;
            *(short *)((char *)a + 0x5e) = t;
            __Actor_SetScript(a, gScript_970__020094c4);
        }
    }
}
