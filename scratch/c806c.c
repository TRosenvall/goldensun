struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __UI_Sanctum(int slot);
extern void __CutsceneWait(int n);
extern void __Func_80b0278(int a, int b);
extern void __Func_80b3284(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_962_200806c(int slot)
{
    struct A *a;
    unsigned short d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x1f, slot);
    } else if (__GetFlag(0x96f)) {
        base = 0x261c;
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25cf);
        __ActorMessage(slot, 0);
    }
}

