extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern int __Func_8092c40(int slot, int a);
extern int __Func_8091c7c(int a, int b);
extern int __Func_8093054(int slot, int a);

void OvlFunc_926_2008484(void)
{
    unsigned char *p;
    unsigned short *s;

    __CutsceneStart();
    if (__GetFlag(0x88f) != 0) {
        __MessageID(0x17d6);
        __Func_8093054(0xc, 0);
        __CutsceneEnd();
        return;
    }
    __MessageID(0x1794);
    __Func_8092c40(0xc, 0);
    if (__Func_8091c7c(0, 0) == 1) {
        s = (unsigned short *)((0xec << 1) + *(unsigned char **)iwram_3001ebc);
        *s = *s + 1;
        __Func_8092c40(0xc, 0);
        if (__Func_8091c7c(0, 0) == 1) {
            p = *(unsigned char **)iwram_3001ebc;
            p += 0xec << 1;
            s = (unsigned short *)p;
            *s = *s + 1;
        }
    }
    __ActorMessage(0xc, 0);
    __CutsceneEnd();
}
