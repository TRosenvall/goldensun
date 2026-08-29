struct A { unsigned char pad00[6]; unsigned short f6; };

extern unsigned int __Random(void);
extern void __Actor_SetAnim(struct A *a, int n);

int OvlFunc_936_2008040(struct A *a)
{
    short *p;
    int v;
    int t;

    p = (short *)((char *)a + 0x66);
    if (*p == 0) {
        v = __Random() << 3 >> 16;
        switch (v) {
        case 0:
            __Actor_SetAnim(a, 3);
            break;
        case 1:
            __Actor_SetAnim(a, 4);
            break;
        case 3:
        case 4:
            a->f6 = a->f6 + (__Random() << 15 >> 16);
            break;
        }
        t = __Random() * 5 << 4 >> 16;
        *(unsigned short *)p = t;
        if (t == 0)
            return 1;
    }
    t = *(unsigned short *)p;
    *(unsigned short *)p = t - 1;
    return 1;
}
