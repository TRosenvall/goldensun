extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __Func_809259c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_945_200c86c(int slot);
extern int OvlFunc_945_200c880(int slot, int v);

void OvlFunc_945_20087f8(void)
{
    unsigned short *q;

    __CutsceneStart();
    if (__GetFlag(0x925)) {
        __Func_809259c(8, 2);
        __MessageID(0x1e13);
        OvlFunc_945_200c86c(8);
        __Func_809280c(8, 0, 0xa);
        __Func_8092c40(8, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x28);
            OvlFunc_945_200c86c(8);
            OvlFunc_945_200c880(8, 0xc0 << 6);
            __ActorMessage(8, 0);
        } else {
            q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
            *q += 2;
            __ActorMessage(8, 0);
            __Func_8092adc(8, 0xc0 << 6, 0);
        }
    } else {
        __MessageID(0x1d4e);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}
