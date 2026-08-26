extern void OvlFunc_916_2008f34(void);
extern void OvlFunc_916_2008f74(void);
extern void OvlFunc_916_2008f54(void);
extern unsigned short OvlFunc_916_2008ecc(unsigned short c, int s);
extern void __Func_8091200(int a, int b);

void OvlFunc_916_2008e64(int scale)
{
    unsigned int i;
    unsigned int u;
    unsigned short *p;
    unsigned int n;

    OvlFunc_916_2008f34();
    i = 0;
    do {
        u = i >> 16;
        if ((unsigned int)(i - 0x110000) > (0xc0 << 11)) {
            if ((unsigned short)(u - 0xc1) > 7) {
                p = (unsigned short *)((0xa0 << 19) + u * 2);
                *p = OvlFunc_916_2008ecc(*p, scale);
            }
        }
        n = i + (0x80 << 9);
        i = n;
    } while (n <= (0xdf << 16));
    OvlFunc_916_2008f74();
    OvlFunc_916_2008f54();
    __Func_8091200(0x80 << 9, 0);
}
