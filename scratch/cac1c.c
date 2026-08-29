struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x24 - 0x14];
    int f24;
    unsigned char pad28[4];
    int f2c;
    unsigned char pad30[8];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern struct A *__Func_8093554(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);

void OvlFunc_897_200ac1c(int x, int y)
{
    struct A *a;
    int fx;
    int fy;

    a = __Func_8093554();
    fy = y << 16;
    fx = x << 16;
    __Func_80933f8(fx, -1, fy, 1);
    __Func_8091200(0, 0);
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    a->f10 = fy;
    a->f38 = 0x80 << 24;
    a->f40 = 0x80 << 24;
    a->f24 = 0;
    a->f2c = 0;
    a->f8 = fx;
    __WaitFrames(5);
    __Func_800fe9c();
    __WaitFrames(5);
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x14);
    __WaitFrames(0x1e);
}
