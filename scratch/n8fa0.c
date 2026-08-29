extern char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_907_2008fa0(void)
{
    char *a;
    int z;
    int p0, p1, p2, p3;

    a = __MapActor_GetActor(8);
    if (a == 0)
        return;
    z = *(int *)(a + 0x10) >> 20;
    if (z == 6) {
        p0 = 0xe;
        __Func_8010704(2, 0, 1, 1, p0, 6);
    } else {
        p1 = 0xe;
        __Func_8010704(0, 0, 1, 1, p1, 6);
    }
    z = *(int *)(a + 0x10) >> 20;
    if (z == 9) {
        p2 = 0xe;
        __Func_8010704(2, 0, 1, 1, p2, 9);
    } else {
        p3 = 0xe;
        __Func_8010704(1, 0, 1, 1, p3, 9);
    }
}
