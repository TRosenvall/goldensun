extern unsigned char iwram_3001ebc[];
extern int __CheckPartyItem(int item);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void OvlFunc_959_2008e30(int n);
extern void __Func_8012330(int a, int b, int c);

void OvlFunc_959_2008e80(void)
{
    char *base;
    short v;

    base = *(char **)iwram_3001ebc;
    if (__CheckPartyItem(0xea) != -1) {
        v = *(short *)(base + (0xb6 << 1));
        OvlFunc_959_2008e30(v - 0x28);
        __PlaySound(0x9d);
        __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
        __Func_8012330(-1, -1, 0xe666);
        __SetFlag(v + (0xcc << 2));
    }
}
