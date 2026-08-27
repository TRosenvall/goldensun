extern char *iwram_3001ebc;
extern int __CheckPartyItem(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void OvlFunc_959_2008d54(int n);

void OvlFunc_959_2008dcc(void)
{
    char *p;
    int m1;
    int n;

    p = iwram_3001ebc;
    m1 = -1;
    if (__CheckPartyItem(0xea) != m1) {
        n = *(short *)(p + (0xb6 << 1));
        OvlFunc_959_2008d54(n - 0x28);
        __PlaySound(0x9d);
        __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
        __Func_8012330(m1, m1, 0xe666);
        __SetFlag(n + 0x32d);
    }
}
