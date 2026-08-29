struct S {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[4];
    unsigned char fe;
};

extern char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);
extern void Func_80a17c4(struct S *s);
extern int _GetUnit(int id);
extern void _Func_801e8b0(int a, void *b, int c, int d);
extern void _Func_801e7c0(int a, void *b, int c, int d);

void Func_80a51d0(void)
{
    char *p;
    struct S **s;
    unsigned short *w;
    void **q;

    p = iwram_3001f2c;
    s = (struct S **)(p + (0x87 << 2));
    w = (unsigned short *)(p + (0xbc << 1));
    _Func_801bcd4(2, *w, (*s)->fe, 0);
    (*s)->f5 = 1;
    (*s)->f6 = 0x70;
    (*s)->f8 = 8;
    Func_80a17c4(*s);
    q = (void **)(p + (0x86 << 1));
    _Func_801e8b0(_GetUnit(*(unsigned char *)(p + 0x21a)), *q, 0x10, 0);
    _Func_801e7c0((*w & 0x1ff) + 0x182, *q, 0x10, 8);
}
