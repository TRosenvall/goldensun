extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern int C40I(int a, int b) __asm__("__Func_8092c40");
extern int __Func_8091c7c(int a, int b);
extern char OvlFunc_941_20092ac(void);
extern char OvlFunc_941_20092c4(void);
extern char OvlFunc_941_20092f0(void);
extern char OvlFunc_941_2009320(void);
extern char OvlFunc_941_2009368(void);
extern char OvlFunc_941_2009394(void);
extern char OvlFunc_941_200941c(void);
extern void OvlFunc_941_200931c(void);
extern void OvlFunc_941_200934c(void);
extern void OvlFunc_941_2009448(void);
extern void OvlFunc_941_2009760(void);

void OvlFunc_941_20091b8(void)
{
    int m;
    int f;

    m = 0x2547;
    __MessageID(m);
    __ActorMessage(0xc, 0);
    m += 1;
    __Func_809280c(1, 0, 0);
    __MessageID(m);
    C40I(1, 0);
    __Func_809280c(2, 0, 0);
    __Func_809280c(3, 0, 0);
    __Func_809280c(0xd, 0, 0);
    __Func_809280c(0xc, 0, 0);
top:
    if (OvlFunc_941_20092ac() == 0)
        goto alt;
retry:
    if (OvlFunc_941_2009320() == 0)
        goto out1;
    f = 0;
    if (OvlFunc_941_200941c() != 0)
        goto check;
set1:
    f = 1;
loop:
    OvlFunc_941_200934c();
    if (__Func_8091c7c(0, 0) == 0)
        goto out1;
check:
    if (OvlFunc_941_2009394() != 0)
        goto out2;
    if (f == 0)
        goto out2;
    goto loop;
alt:
    if (OvlFunc_941_20092c4() == 0)
        goto more;
    if (OvlFunc_941_20092f0() != 0)
        goto out2;
    goto set1;
more:
    if (OvlFunc_941_2009368() != 0)
        goto retry;
    m = 0x254b;
    __MessageID(m);
    __ActorMessage(2, 0);
    m += 1;
    __MessageID(m);
    __Func_8092c40(1, 0);
    goto top;
out1:
    OvlFunc_941_2009760();
    return;
out2:
    OvlFunc_941_200931c();
    OvlFunc_941_2009448();
}
