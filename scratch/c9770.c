extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_934_2009938(int a, int b, int c);

void OvlFunc_934_2009770(void)
{
    int e5, e6;

    e5 = 0x17;
    e6 = 0x22;
    __Func_8010704(0, 0x22, 0xd, 3, e5, e6);
    if (__GetFlag(0x301)) {
        OvlFunc_934_2009938(0xb, 0x23, 0x23);
        __Func_8010704(0x18, 0x22, 1, 3, e5, e6);
    } else {
        OvlFunc_934_2009938(0xb, 0x17, 0x23);
        __Func_8010704(0x18, 0x22, 1, 3, 0x23, e6);
    }
}
