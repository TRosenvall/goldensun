extern void __PlaySound(int id);
extern int __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_884_2008714(int n);

void OvlFunc_884_2008780(void)
{
    int p;
    int r;

    __PlaySound(0xbc);
    __CopyMapTiles(0, 0x3f, 0x33, 8, 2, 2);
    __WaitFrames(0xa);
    __CopyMapTiles(2, 0x3f, 0x33, 8, 2, 2);
    __WaitFrames(0xa);
    r = 0x99;
    p = 0xb0;
    r <<= 1;
    p <<= 1;
    __Func_80921c4(0, p, r);
    __Func_8092b08(0, 3);
    p = 0xb0;
    p <<= 1;
    __Func_80921c4(0, p, 0x94 << 1);
    OvlFunc_884_2008714(2);
}
