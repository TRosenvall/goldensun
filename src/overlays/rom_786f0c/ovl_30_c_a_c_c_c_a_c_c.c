extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092a1c(int a, int b, void *c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c(int a, int b);
extern void __Func_8091a58(int a, int b);
extern unsigned char ActorCmd_ARRAY_886__020092fc[];

void OvlFunc_886_20081e8(void)
{
    int s;
    int m;

    s = 0x80 << 9;
    __CutsceneStart();
    if (__GetFlag(0x81b) != 0) {
        __MessageID(0x11a6);
        __ActorMessage(0x14, 0);
        __Func_8092a1c(0x14, s, ActorCmd_ARRAY_886__020092fc);
    } else {
        m = 0x11a4;
        __MessageID(m);
        m += 1;
        __Func_8093040(0x14, 0, 0x14);
        __Func_801776c(m, 1);
        __Func_8091a58(0xb4, 0);
        __SetFlag(0x81b);
    }
    __CutsceneEnd();
}
