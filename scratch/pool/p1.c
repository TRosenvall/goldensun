typedef struct { unsigned char pad00[0x10]; int f10; unsigned char pad14[0x2ac]; } GlobalState;
struct X { unsigned char pad00[0xc]; int f0c; };

extern unsigned char iwram_3001f2c[];
extern GlobalState gState;
extern void _Func_801e7c0(int a, int b, int c, int d);
extern void _Func_801ea08(int a, int b, int c, int d, int e);

void Func_80b10cc(void)
{
    int v;

    v = (*(struct X **)iwram_3001f2c)->f0c;
    if (v != 0) {
        _Func_801e7c0(0xc8a, v, 0, 0);
        _Func_801ea08(gState.f10, 6, v, 0x20, 8);
    }
}
