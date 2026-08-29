extern int L269c __asm__(".L269c");
extern int __Random(void);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);

void OvlFunc_895_2009ac8(void)
{
    int *p;
    int v;
    int m1, m2;
    int e;
    int a1, a2, a3;

    e = 0xe666;
    m1 = -1;
    m2 = -1;
    a1 = 0x80 << 9;
    a2 = 0x80 << 10;
    a3 = 0x80 << 9;
    p = &L269c;
    v = *p;
    if (v != 0) {
        v -= 1;
        *p = v;
        if (v == 0x28)
            __Func_8012330(m1, m2, e);
    } else {
        if (((unsigned int)(__Random() * 120) >> 16) == 0) {
            __PlaySound(0x8a);
            __Func_8012330(a1, a2, a3);
            *p = 0x50;
        }
    }
}
