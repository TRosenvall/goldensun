extern int L5fa4 __asm__(".L5fa4");
extern void __Func_809280c(int a, int b, int c);
extern void OvlFunc_959_2009c4c(int a);
extern void OvlFunc_959_2009ca4(int a);
extern void OvlFunc_959_2009cf0(int a);
extern void OvlFunc_959_2009d60(int a);

void OvlFunc_959_2009be4(int a)
{
    __Func_809280c(a, 0, 0);
    __Func_809280c(0, a, 0);
    switch (L5fa4 & 3) {
    case 0:
        OvlFunc_959_2009c4c(a);
        break;
    case 1:
        OvlFunc_959_2009ca4(a);
        break;
    case 2:
        OvlFunc_959_2009cf0(a);
        break;
    case 3:
        OvlFunc_959_2009d60(a);
        break;
    default:
        OvlFunc_959_2009ca4(a);
        break;
    }
}
