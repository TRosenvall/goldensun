extern unsigned char L3c9c[] __asm__(".L3c9c");
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __LoadFieldActors(void *p);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_80931ec(int a, int b, int c, int d, int e, int f, int g,
                           int h, int i, int j, int k);

void OvlFunc_888_20084e8(void)
{
    unsigned char *p;
    int a1, a2, a3, a4, a5, a6;

    __CutsceneStart();
    __LoadFieldActors(L3c9c);
    __WaitFrames(1);
    __MessageID(0x1bfd);
    __Func_8092c40(9, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __ActorMessage(9, 0);
    } else {
        p = iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
        a1 = 1;
        a2 = 3;
        a3 = 7;
        a4 = 0x10;
        a5 = 0xe;
        a6 = 0;
        __Func_80931ec(2, a4, 1, 0x18, a1, a2, a3, a4, a1, a5, a6);
        __ActorMessage(9, 0);
    }
    __CutsceneEnd();
}
