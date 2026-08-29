extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern int __Func_80925cc(int slot, int a);
extern void __Func_8093040(int slot, int a, int b);
extern void __Func_8092950(int slot, int a);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_968_2008f38(void)
{
    unsigned int n;
    unsigned int d;
    int m;
    int k;

    __CutsceneStart();
    __Func_80925cc(8, 3);
    __MessageID(0x266d);
    n = 0xa;
    d = 8;
    __Func_8093040(8, 0, 0x14);
    do {
        __Func_8092950(8, 0xf);
        __WaitFrames(2);
        __Func_8092950(8, 0);
        __WaitFrames(d);
        if (d > 3)
            d--;
    } while (--n != 0);
    __SetFlag(0x981);
    __MapActor_SetPos(8, 0, 0);
    m = 7;
    k = 0x10;
    __Func_8010704(7, 0x11, 2, 1, m, k);
    __CutsceneEnd();
}
