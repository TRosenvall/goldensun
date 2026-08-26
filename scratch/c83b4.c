extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8093054(int slot, int n);
/* __Func_8092c40 intentionally implicit */

void OvlFunc_884_20083b4(void)
{
    char *base;

    __CutsceneStart();
    if (__GetFlag(0x87a)) {
        __MessageID(0x1be8);
        __Func_8092c40(0xf, 0);
        if (__Func_8091c7c(0, 0) == 1) {
            __ActorMessage(0xf, 0);
        } else {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1)))++;
            __Func_8093054(0xf, 0);
        }
    } else if (__GetFlag(0x815)) {
        __MessageID(0x1191);
        __Func_8093054(0xb, 0);
    } else {
        __MessageID(0xea8);
        __Func_8093054(0xb, 0);
    }
    __CutsceneEnd();
}
