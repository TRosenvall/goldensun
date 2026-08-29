struct B { unsigned char pad00[0x14]; unsigned short f14; };

extern struct B *iwram_3001e70;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_932_200a428(void)
{
    struct B *p;
    unsigned char *g;
    int e;
    int f;

    if (__GetFlag(0x8fe)) {
        p = iwram_3001e70;
        p->f14 &= 0xfdff;
    } else {
        e = 0x35;
        f = 0x2a;
        __Func_8010704(0x34, 0x2a, 1, 1, e, f);
    }
    g = gState;
    if ((unsigned short)(*(unsigned short *)(g + (0xe1 << 1)) - 6) <= 1)
        __ClearFlag(0x12f);
}
