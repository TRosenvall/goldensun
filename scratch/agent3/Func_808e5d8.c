extern unsigned char gState[];
extern unsigned char *_GetMoveInfo(int id);
extern int  GetFieldActor(int a);
extern int  Func_808e4b4(int a, int b, int *v);
extern void Func_8096fb0(int a, int b);
extern void Func_80970f8(int a, int b);
extern int Func_8096b28(int a, int b, int c);
extern void Func_8096af0(void);
extern void Func_8097174(void);
extern void Func_8097194(void);

int Func_808e5d8(int arg)
{
    unsigned char *gs;
    int v;
    int id;
    unsigned int n;
    int k;
    int a;
    int b;

    id = 0x3ff & arg;
    n = arg;
    n >>= 10;
    n &= 0xf;
    k = _GetMoveInfo(id)[0xc];
    gs = gState;
    gs += 0xfa << 1;
    GetFieldActor(*(int *)gs);
    a = Func_808e4b4(0x30000005, k, &v);
    b = Func_808e4b4(0x20000005, k, &v);
    Func_8096fb0(id, 0);
    Func_80970f8(*(int *)gs, v);
    Func_8096b28(a, n, v);
    Func_8096af0();
    Func_8097174();
    Func_8096b28(b, n, v);
    Func_8097194();
    return 0;
}
