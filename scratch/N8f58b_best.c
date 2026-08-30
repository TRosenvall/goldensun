struct Blk {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
};

extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2008ddc(int a, int *b, int *c, struct Blk *d, int *e, int *f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_947_2008528(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_947_2008f58(int a)
{
    struct Blk t;
    int v14;
    int v10;
    int vc;
    int v8;
    unsigned char *e;
    int p;
    int q;

    e = __MapActor_GetActor(a);
    if (OvlFunc_947_2008ddc(a, &v14, &v10, &t, &vc, &v8) == 0)
        return 0;
    q = t.f10;
    p = t.f8;
    __Func_8010704(vc + p, v8 + q, v14, v10, p, q);
    OvlFunc_947_2008528(0, t.f8, t.f10, v14, v10, 0xff);
    __Actor_SetAnim(e, 1);
    e[0x23] &= 0xfd;
    return 1;
}
