extern void OvlFunc_915_2008d5c(void);
extern void OvlFunc_915_2008d9c(void);
extern void OvlFunc_915_2008d7c(void);
extern unsigned short OvlFunc_915_2008cf4(unsigned short c, int s);
extern void __Func_8091200(int a, int b);

void OvlFunc_915_2008c8c(int scale)
{
    unsigned int i;
    unsigned int u;
    unsigned short *p;
    unsigned int n;

    OvlFunc_915_2008d5c();
    i = 0;
    do {
        u = i >> 16;
        if ((unsigned int)(i - 0x110000) > (0xc0 << 11)) {
            if ((unsigned short)(u - 0xc1) > 7) {
                p = (unsigned short *)((0xa0 << 19) + u * 2);
                *p = OvlFunc_915_2008cf4(*p, scale);
            }
        }
        n = i + (0x80 << 9);
        i = n;
    } while (n <= (0xdf << 16));
    OvlFunc_915_2008d9c();
    OvlFunc_915_2008d7c();
    __Func_8091200(0x80 << 9, 0);
}
