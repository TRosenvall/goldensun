extern void OvlFunc_917_2009838(void);
extern void OvlFunc_917_2009878(void);
extern void OvlFunc_917_2009858(void);
extern unsigned short OvlFunc_917_20097d0(unsigned short c, int s);
extern void __Func_8091200(int a, int b);

void OvlFunc_917_2009768(int scale)
{
    unsigned int i;
    unsigned int u;
    unsigned short *p;
    unsigned int n;

    OvlFunc_917_2009838();
    i = 0;
    do {
        u = i >> 16;
        if ((unsigned int)(i - 0x110000) > (0xc0 << 11)) {
            if ((unsigned short)(u - 0xc1) > 7) {
                p = (unsigned short *)((0xa0 << 19) + u * 2);
                *p = OvlFunc_917_20097d0(*p, scale);
            }
        }
        n = i + (0x80 << 9);
        i = n;
    } while (n <= (0xdf << 16));
    OvlFunc_917_2009878();
    OvlFunc_917_2009858();
    __Func_8091200(0x80 << 9, 0);
}
