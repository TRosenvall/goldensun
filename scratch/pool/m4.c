extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_953_2009c5c(int slot, int v);
extern void OvlFunc_953_2009c48(int slot);

void OvlFunc_953_200960c(void)
{
    char *base;
    int *p;
    int off;

    __CutsceneStart();
    base = *(char **)iwram_3001ebc;
    *(int *)(base + 0x1c0) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    OvlFunc_953_2009c5c(0x11, 0xa0 << 7);
    __MessageID(0x206e);
    if (__GetFlag(0x8a4)) {
        base = *(char **)iwram_3001ebc;
        (*(unsigned short *)(base + (0xec << 1)))++;
    }
    OvlFunc_953_2009c48(0x11);
    OvlFunc_953_2009c5c(0x11, 0xc0 << 6);
    __SetFlag(0x8a3);
    __CutsceneEnd();
}
