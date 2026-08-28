extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2008ddc(int a, int *b, int *c, int *d, int *e, int *f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

int OvlFunc_947_2008ec8(int slot)
{
    int v[6];
    int x;
    int y;
    int bb;
    int aa;
    unsigned char *a;
    unsigned char *p;
    int s0, s1;

    a = __MapActor_GetActor(slot);
    if (OvlFunc_947_2008ddc(slot, &x, &y, v, &bb, &aa) == 0)
        return 0;
    s0 = v[2];
    s1 = v[4];
    __Func_8010704(2, 2, x, y, s0, s1);
    __Actor_SetAnim(a, 4);
    p = a + 0x23;
    *p |= 2;
    if ((unsigned int)x > (unsigned int)y)
        __CopyMapTiles(0x46, 0x28, v[2] + 0x20, v[4] + 2, x, y);
    else
        __CopyMapTiles(0x44, 0x28, v[2] + 0x20, v[4] + 2, x, y);
    return 1;
}
