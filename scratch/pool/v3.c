typedef unsigned short u16; typedef unsigned int u32; typedef volatile unsigned short vu16;
extern void Func_80042c8(void *fn);
extern void Func_800c62c(void); extern void Func_800c880(void);
extern void _Func_8091200(int a, int b); extern void _Func_8091254(int a);
extern void WaitFrames(int n);
void Func_800c5b4(void)
{
    vu16 *p; u32 v;
    Func_80042c8(Func_800c62c);
    Func_80042c8(Func_800c880);
    _Func_8091200(0x80 << 9, 1);
    _Func_8091254(1);
    WaitFrames(1);
    p = (vu16 *)(0x80 << 19);
    v = *p & 0xf1ff;
    *p = v | 0x1000;
}
