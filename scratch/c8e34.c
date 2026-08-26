struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80b3284(int inn, int slot);
extern void __Func_80925cc(int slot, int n);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093054(int slot, int n);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_887_2008e34(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0x2000) > (0x90 << 8)) {
        __Func_80b3284(0, 0xd);
        return;
    }
    __CutsceneStart();
    if (__GetFlag(0x87a)) {
        __Func_80925cc(0xd, 2);
        __Func_809280c(0xd, 0, 0xa);
        if (__GetFlag(0xc0 << 2) == 0) {
            __MessageID(0x1c14);
            __ActorMessage(0xd, 0);
            __SetFlag(0xc0 << 2);
        }
        __MessageID(0x1c15);
        __Func_8093054(0xd, 0);
        __Func_8092adc(0xd, 0x90 << 8, 0xa);
    } else {
        if (__GetFlag(0x815))
            __MessageID(0x11a9);
        else
            __MessageID(0xf58);
        __ActorMessage(0xd, 0);
    }
    __CutsceneEnd();
}
