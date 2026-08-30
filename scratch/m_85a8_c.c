extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_80b0278(int a, int b);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);

void OvlFunc_950_20085a8(int slot)
{
    unsigned char *a;
    unsigned int t;
    int base;

    a = __MapActor_GetActor(0);
    t = (*(unsigned short *)(a + 6) + (0x80 << 6)) & 0xffffc000;
    if (t << 16 == (0xc0 << 24)) {
        __Func_80b0278(0x1a, slot);
        return;
    }
    if (__GetFlag(0x95 << 4)) {
        base = 0x2389;
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) != 0) {
            __MessageID(base + 2);
        } else {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        }
        __ActorMessage(slot, 0);
        return;
    }
    if (__GetFlag(0x962)) {
        __MessageID(0x2219);
        __ActorMessage(slot, 0);
        return;
    }
    __MessageID(0x1fd2);
    __ActorMessage(slot, 0);
    __MapActor_Emote(slot, 0x83 << 1, 0);
    __CutsceneWait(0x28);
    __ActorMessage(slot, 0);
}
