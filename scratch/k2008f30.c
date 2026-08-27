extern char *iwram_3001ebc;
extern int __CheckPartyItem(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void OvlFunc_959_2008ee0(int n);

void OvlFunc_959_2008f30(void)
{
    char *p;
    int n;
    int a1;
    int a2;
    int a3;
    int e;

    a1 = 0xc0 << 10;
    a2 = 0xc0 << 10;
    a3 = 0x80 << 9;
    e = 0xe666;
    p = iwram_3001ebc;
    if (__CheckPartyItem(0xea) != -1) {
        n = *(short *)(p + (0xb6 << 1));
        OvlFunc_959_2008ee0(n - 0x28);
        __PlaySound(0x9d);
        __Func_8012330(a1, a2, a3);
        __Func_8012330(-1, -1, e);
        __SetFlag(n + 0x332);
    }
}
