extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_947_200a1ac(void)
{
    unsigned char *a;
    unsigned char *p;
    int v;
    int mask;
    int s1;
    int s2;

    a = __MapActor_GetActor(0xe);
    p = __MapActor_GetActor(0xd);
    v = 0x80 << 9;
    *(int *)(p + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xd) + 0x1c) = v;
    p = *(unsigned char **)(__MapActor_GetActor(0xd) + 0x50);
    mask = -13;
    p[9] = (p[9] & mask) | 8;
    p = *(unsigned char **)(a + 0x50);
    p[9] = (p[9] & mask) | 8;
    *(int *)(a + 0x34) = 0x6666;
    *(int *)(a + 0x30) = 0xcccc;
    __Actor_TravelTo(a, *(int *)(a + 8), 0x80 << 14, *(int *)(a + 0x10));
    __MapActor_WaitMovement(0xe);
    s1 = 0x16;
    s2 = 0x10;
    __Func_8010704(0x14, 0xe, 1, 1, s1, s2);
}
