extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_903_2008348(void)
{
    unsigned char *a;
    int v;
    int e1, f1, e2, f2, f3;

    a = __MapActor_GetActor(9);
    v = *(int *)(a + 8) / (1 << 20);
    __ClearFlag(0x861);
    __ClearFlag(0x862);
    if (v == 0xf) {
        e1 = 0x10;
        f1 = 0x12;
        __Func_8010704(0x2f, 0x12, 1, 2, e1, f1);
    } else if (v == 0x10) {
        f3 = 0x12;
        __Func_8010704(0x30, 0x12, 1, 2, v, f3);
        __SetFlag(0x861);
    } else {
        e2 = 0x10;
        f2 = 0x12;
        __Func_8010704(0x2f, 0x12, 1, 2, e2, f2);
        __SetFlag(0x862);
    }
}
