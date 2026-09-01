extern unsigned char gState[];
extern int __GetFlag(int id);
extern void OvlFunc_922_2009004(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_922_20095dc(void)
{
    int v;
    int off;
    int p1, q1, p2, q2, p3, q3, p4, q4;
    int p5, q5, p6, q6, p7, q7, p8, q8;

    off = 0xe1 << 1;
    v = *(unsigned short *)(gState + off);
    if ((unsigned short)(v - 1) <= 1) {
        p1 = 0xe;
        q1 = 0xa;
        __Func_8010704(0x16, 0x14, 9, 8, p1, q1);
    } else {
        p2 = 7;
        q2 = 0x2d;
        __Func_8010704(0x14, 0x2d, 0xb, 4, p2, q2);
    }
    if (__GetFlag(0x313)) {
        OvlFunc_922_2009004(8, 0x14, 0x11);
        p3 = 0x13;
        q3 = 0xa;
        __Func_8010704(0x13, 0xb, 3, 1, p3, q3);
    } else {
        OvlFunc_922_2009004(8, 0x14, 0xa);
        p4 = 0x13;
        q4 = 0x11;
        __Func_8010704(0x13, 0xb, 3, 1, p4, q4);
    }
    if (__GetFlag(0xc5 << 2)) {
        OvlFunc_922_2009004(9, 0xe, 0x10);
        p5 = 0x16;
        q5 = 0xf;
        __Func_8010704(0x10, 0xf, 1, 3, p5, q5);
    } else {
        OvlFunc_922_2009004(9, 0x16, 0x10);
        p6 = 0xe;
        q6 = 0xf;
        __Func_8010704(0x10, 0xf, 1, 3, p6, q6);
    }
    if (__GetFlag(0x315)) {
        OvlFunc_922_2009004(0xa, 0x11, 0x2e);
        p7 = 7;
        q7 = 0x2d;
        __Func_8010704(0xf, 0xf, 1, 3, p7, q7);
    } else {
        OvlFunc_922_2009004(0xa, 7, 0x2e);
        p8 = 0x11;
        q8 = 0x2d;
        __Func_8010704(0xf, 0xf, 1, 3, p8, q8);
    }
}
