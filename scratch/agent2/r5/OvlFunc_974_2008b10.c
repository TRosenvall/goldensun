extern void __Func_801776c(int a, int b);
extern int __ModifyHP(int who, int amount);
extern int __ModifyPP(int who, int amount);
extern unsigned char *__GetUnit(int who);
extern void __CalcStats(int who);

void OvlFunc_974_2008b10(void)
{
    unsigned char *p;

    __Func_801776c(0xc1b, 1);
    __ModifyHP(0, -0x64);
    __ModifyHP(1, -0x64);
    __ModifyHP(2, -0x21);
    __ModifyHP(3, -0x64);
    __ModifyPP(0, -0x32);
    __ModifyPP(1, -0x28);
    __ModifyPP(2, -0x23);
    __ModifyPP(3, -0x14);
    p = __GetUnit(0);
    p[0x131] = 1;
    p[0x140] = 1;
    p = __GetUnit(1);
    p[0x130] = 1;
    p[0x131] = 2;
    __CalcStats(0);
    __CalcStats(1);
    __CalcStats(3);
    __CalcStats(2);
}
