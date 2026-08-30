extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern int __Func_8011f54(int a, int b, int c);
extern void __SetFlagByte(int id, int v);

void OvlFunc_954_20081a8(void)
{
    unsigned char *e;
    int r;
    int s1;
    int s2;
    int t1;
    int t2;
    int u1;
    int fid;
    int u2;

    s1 = 0x17;
    s2 = 0xc;
    __Func_8010704(0x1b, 0xd, 3, 1, s1, s2);
    e = __MapActor_GetActor(9);
    r = __Func_8011f54(0, *(int *)(e + 8), *(int *)(e + 0x10));
    if (*(int *)(e + 0xc) == 0 && r == 0) {
        e[0x23] = 2;
        e[0x55] = r;
        t1 = *(int *)(e + 8) >> 20;
        t2 = *(int *)(e + 0x10) >> 20;
        __Func_8010704(0xe, 0xd, 1, 1, t1, t2);
    }
    e = __MapActor_GetActor(0xa);
    fid = 0xc4 << 2;
    __SetFlagByte(fid, *(int *)(e + 8) >> 20);
    u1 = *(int *)(e + 8) >> 20;
    u2 = *(int *)(e + 0x10) >> 20;
    __Func_8010704(0xe, 0xd, 1, 1, u1, u2);
}
