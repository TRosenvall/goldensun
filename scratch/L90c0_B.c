extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8012350(void);
extern void __Func_8091e9c(int n);

void OvlFunc_924_20090c0(void)
{
    unsigned int off;
    unsigned int q;
    int a1, a2, a3;
    int m, n;

    a1 = 0x80 << 9;
    a2 = 0x80 << 9;
    a3 = 0x80 << 9;
    if (__GetFlag(0xc4 << 2) != 0 && __GetFlag(0x311) != 0 && __GetFlag(0x312) != 0) {
        __SetFlag(0x876);
        __CutsceneWait(0x1e);
        __Func_8012330(a1, a2, a3);
        __PlaySound(0x8d);
        __CutsceneWait(0x3c);
        q = (unsigned int)iwram_3001ebc;
        off = 0xe0;
        off <<= 1;
        q += off;
        off -= 0xc0;
        *(int *)q = off;
        __MapTransitionOut();
        __WaitMapTransition();
        __PlaySound(0x121);
        m = 1;
        n = 1;
        m = -m;
        n = -n;
        __Func_8012330(m, n, 0xe666);
        __Func_8012350();
        __Func_8091e9c(0xd);
    } else {
        __ClearFlag(0x876);
    }
}
