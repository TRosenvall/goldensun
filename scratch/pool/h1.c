extern void OvlFunc_914_2008b8c(void);
extern void OvlFunc_914_2008bcc(void);
extern void OvlFunc_914_2008bac(void);
extern unsigned short OvlFunc_914_2008b24(unsigned short c, int s);
extern void __Func_8091200(int a, int b);

void OvlFunc_914_2008abc(int scale)
{
    unsigned int i;
    unsigned int u;
    unsigned short *p;
    unsigned int n;

    OvlFunc_914_2008b8c();
    i = 0;
    do {
        u = i >> 16;
        if ((unsigned int)(i - 0x110000) > (0xc0 << 11)) {
            if ((unsigned short)(u - 0xc1) > 7) {
                p = (unsigned short *)((0xa0 << 19) + u * 2);
                *p = OvlFunc_914_2008b24(*p, scale);
            }
        }
        n = i + (0x80 << 9);
        i = n;
    } while (n <= (0xdf << 16));
    OvlFunc_914_2008bcc();
    OvlFunc_914_2008bac();
    __Func_8091200(0x80 << 9, 0);
}
