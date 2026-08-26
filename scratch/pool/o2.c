extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_DoAnim(int slot, int a);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_963_2008730(void)
{
    char *base;

    __CutsceneStart();
    if (__GetFlag(0x89f)) {
        __MessageID(0x2668);
        __ActorMessage(9, 0);
    } else {
        __MessageID(0x264e);
        __Func_8092c40(9, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __ActorMessage(9, 0);
            __MapActor_DoAnim(9, 4);
            __ActorMessage(9, 0);
        } else {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1))) += 2;
            __ActorMessage(9, 0);
        }
    }
    __CutsceneEnd();
}
