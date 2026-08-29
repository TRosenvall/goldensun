struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80b0278(int shop, int slot);
extern void OvlFunc_886_20081e8(void);

void OvlFunc_886_20085d4(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0xa001) <= 0x3ffe) {
        __Func_80b0278(3, 0x14);
    } else if (__GetFlag(0x87a)) {
        __CutsceneStart();
        __MessageID(0x1c0a);
        __ActorMessage(0x14, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x815)) {
        OvlFunc_886_20081e8();
    } else {
        __CutsceneStart();
        __MessageID(0xf55);
        __ActorMessage(0x14, 0);
        __CutsceneEnd();
    }
}
