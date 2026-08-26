extern int L4834 __asm__(".L4834");
extern int L4838 __asm__(".L4838");
extern void __Func_809ad90(int n);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __StopTask(void *fn);
extern void OvlFunc_955_2008714(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_20089b0(void)
{
    int t;

    __Func_809ad90(0x1f);
    __SetFlag(0xcd << 2);
    if (L4834 != 0)
        L4838 = 0;
    __WaitFrames(0x1e);
    __WaitFrames(1);
    __StopTask(OvlFunc_955_2008714);
    t = 0x3a;
    __Func_8010704(0x3a, 0x1c, 7, 1, t, 0xd);
    __Func_8010704(0x39, 0xb, 1, 1, t, 0xb);
}
