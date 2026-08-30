extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_907_2008fa0(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(8);
    if (a == 0)
        return;
    if (*(int *)(a + 0x10) >> 20 == 6) {
        __Func_8010704(2, 0, 1, 1, 0xe, 6);
    } else {
        int e0 = 0xe;
        int f0 = 6;
        __Func_8010704(0, 0, 1, 1, e0, f0);
    }
    if (*(int *)(a + 0x10) >> 20 == 9) {
        __Func_8010704(2, 0, 1, 1, 0xe, 9);
    } else {
        int e1 = 0xe;
        int f1 = 9;
        __Func_8010704(1, 0, 1, 1, e1, f1);
    }
}
