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
    unsigned char *p;
    int off;

    if (__GetFlag(0xc4 << 2) && __GetFlag(0x311) && __GetFlag(0x312)) {
        __SetFlag(0x876);
        __CutsceneWait(0x1e);
        __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
        __PlaySound(0x8d);
        __CutsceneWait(0x3c);
        p = iwram_3001ebc;
        off = 0xe0 << 1;
        p = p + off;
        off -= 0xc0;
        *(int *)p = off;
        __MapTransitionOut();
        __WaitMapTransition();
        __PlaySound(0x121);
        __Func_8012330(-1, -1, 0xe666);
        __Func_8012350();
        __Func_8091e9c(0xd);
    } else {
        __ClearFlag(0x876);
    }
}
