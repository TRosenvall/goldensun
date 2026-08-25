extern unsigned int iwram_3001e40;
extern void __Func_80929d8(void *a, int n);
extern void __PlaySound(int id);

int OvlFunc_968_20085ac(void *a)
{
    if ((iwram_3001e40 & 3) == 0)
        __Func_80929d8(a, 7);
    else
        __Func_80929d8(a, 0);
    if ((iwram_3001e40 & 7) == 0)
        __PlaySound(0x8a);
    return 0;
}
