extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_949_20082f0(void)
{
    char *q;
    char *p;
    unsigned short *f;
    int saved;
    int id;
    int t;

    q = iwram_3001ebc;
    p = __MapActor_GetActor(0x10);
    f = (unsigned short *)(p + 0x64);
    saved = *(short *)(p + 6);
    __CutsceneStart();
    t = *f;
    *f = t | 2;
    if (*(short *)(q + (0xbf << 1)) == 0) {
        if (__GetFlag(0x95 << 4))
            id = 0x2365;
        else if (__GetFlag(0x962))
            id = 0x21e2;
        else
            id = 0x1f95;
    } else {
        if (__GetFlag(0x95 << 4))
            id = 0x2371;
        else if (__GetFlag(0x962))
            id = 0x21f5;
        else
            id = 0x1faa;
    }
    __MessageID(id);
    __MapActor_SetAnim(0x10, 0);
    __Func_8092848(0x10, 0, 2);
    __Func_8093040(0x10, 0, 0xa);
    *(short *)(p + 6) = saved;
    __WaitFrames(1);
    *f &= 1;
    __CutsceneEnd();
}
