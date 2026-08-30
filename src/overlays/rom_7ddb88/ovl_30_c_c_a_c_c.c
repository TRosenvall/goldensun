extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlagByte(int id, int v);

void OvlFunc_955_20080c0(void)
{
    int s1;
    int s2;
    int k;
    int v;

    s1 = 0xe;
    s2 = 0xb;
    __Func_8010704(0x64, 0xb, 0xc, 4, s1, s2);
    v = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    __SetFlagByte(0xd0 << 2, v);
    k = 0x10;
    __Func_8010704(0x47, 0x10, 1, 1, v, k);
    v = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    __SetFlagByte(0xd2 << 2, v);
    __Func_8010704(0x47, 0x10, 1, 1, v, k);
    v = *(int *)(__MapActor_GetActor(0xe) + 8) >> 20;
    __SetFlagByte(0xd4 << 2, v);
    __Func_8010704(0x47, 0x10, 1, 1, v, k);
}
