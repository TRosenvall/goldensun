extern void __Func_8010704(int, int, int, int, int, int);
extern unsigned char *__MapActor_GetActor(int);

void OvlFunc_955_200862c(void)
{
    int s;
    int v1, v2, v3;

    s = 0xb;
    __Func_8010704(0x64, 0xb, 0xc, 4, 0xe, s);
    v1 = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    __Func_8010704(0xd, 0x1c, 1, 4, v1, s);
    v2 = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    __Func_8010704(0xd, 0x1c, 1, 4, v2, s);
    v3 = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    __Func_8010704(0xd, 0x1c, 4, 1, 0x12, v3);
}
