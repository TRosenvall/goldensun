struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern unsigned char gScript_943__0200c4d8[];
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern unsigned int __Random(void);
extern void __Func_80925cc(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);

void OvlFunc_943_2008a48(void)
{
    struct A *a;

    __CutsceneStart();
    if (__GetFlag(0x925)) {
        __MessageID(0x1e08);
        __ActorMessage(0x15, 0);
    } else if (__GetFlag(0x922)) {
        __Func_80925cc(0x15, 2);
        __MessageID(0x1d6f);
        __ActorMessage(0x15, 0);
        a = __MapActor_GetActor(0x15);
        a->f64 = (__Random() * 0x5a >> 16) + 0x3c;
        __MapActor_SetBehavior(0x15, gScript_943__0200c4d8);
    } else {
        __MapActor_Emote(0x15, 0x103, 0);
        __Func_809259c(0x15, 3);
        __MessageID(0x1d36);
        __ActorMessage(0x15, 0);
    }
    __CutsceneEnd();
}

void OvlFunc_943_2008af0(void)
{
    struct A *a;

    __CutsceneStart();
    if (__GetFlag(0x925)) {
        __MessageID(0x1e09);
        __ActorMessage(0x18, 0);
    } else if (__GetFlag(0x922)) {
        __Func_80925cc(0x18, 2);
        __MessageID(0x1d70);
        __ActorMessage(0x18, 0);
        a = __MapActor_GetActor(0x18);
        a->f64 = (__Random() * 0x5a >> 16) + 0x3c;
        __MapActor_SetBehavior(0x18, gScript_943__0200c4d8);
    } else {
        __MapActor_Emote(0x18, 0x103, 0);
        __Func_809259c(0x18, 3);
        __MessageID(0x1d37);
        __ActorMessage(0x18, 0);
    }
    __CutsceneEnd();
}
