extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);

void OvlFunc_883_2008ba8(void)
{
    int e;
    int s;
    int t;
    int f2;

    e = 0x103;
    s = 0x80 << 8;
    t = 0x81 << 1;
    __CutsceneStart();
    if (__GetFlag(0x807) == 0) {
        f2 = 0x807;
        __SetFlag(f2);
        __MessageID(0xf63);
        __MapActor_Emote(0x12, e, 0);
        __Func_8092848(0, 0x12, 0x14);
        __Func_8093040(0x12, 0, 6);
        __Func_8092adc(0x12, s, 0x1e);
        __MapActor_Jump(0x12, 2, 0x14);
        __Func_8093040(0x12, 0, 6);
        __Func_8092848(0x12, 0, 0xa);
        __MapActor_Emote(0x12, e, 0);
        __Func_8093040(0x12, 0, 0xa);
        __MapActor_Emote(0, t, 0x3c);
    } else {
        __MapActor_Emote(0x12, e, 0);
        __MessageID(0xf66);
        __Func_8093040(0x12, 0, 0x14);
    }
    __CutsceneEnd();
}
