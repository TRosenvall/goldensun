extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern void __Func_801776c(int a, int b);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_965_200a4d0(void);

void OvlFunc_965_200a5c8(void)
{
    char *p;
    int s0;
    int s1;

    p = iwram_3001ebc;
    __CutsceneStart();
    p += 0xcb8;
    if (*(short *)p != 0) {
        if (__GetFlag(0x985) == 0) {
            __Func_801776c(0x1528, 1);
            __PlaySound(0x9b);
            s0 = 0x11;
            s1 = 0x4e;
            __Func_8010788(0x23, 0x4e, 1, 2, s0, s1);
            __CutsceneWait(0xa);
            __Func_8010788(0x22, 0x4e, 1, 2, s0, s1);
            __CutsceneWait(0xa);
            OvlFunc_965_200a4d0();
        }
    } else {
        __MessageID(0x2756);
        __ActorMessage(-1, 0);
    }
    __CutsceneEnd();
}
