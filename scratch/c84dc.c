struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80b0278(int shop, int slot);
extern void __Func_8093054(int slot, int n);

void OvlFunc_886_20084dc(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0xa001) <= 0x3ffe) {
        __Func_80b0278(1, 0x15);
    } else {
        __CutsceneStart();
        if (__GetFlag(0x87a)) {
            __MessageID(0x1c06);
            __Func_8093054(0x15, 0);
        } else {
            if (__GetFlag(0x815))
                __MessageID(0x11a2);
            else
                __MessageID(0xf53);
            __ActorMessage(0x15, 0);
        }
        __CutsceneEnd();
    }
}
