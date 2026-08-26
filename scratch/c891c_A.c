struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __UI_Sanctum(int slot);

void OvlFunc_950_200891c(int slot)
{
    struct A *a;

    a = __MapActor_GetActor(0);
    if ((unsigned short)((a->f6 + 0x2000) & ~0x3fff) == 0xc000) {
        __UI_Sanctum(slot);
    } else if (__GetFlag(0x95 << 4)) {
        __MessageID(0x23bf);
        __ActorMessage(slot, 0);
    } else if (__GetFlag(0x962)) {
        __MessageID(0x2231);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x1feb);
        __ActorMessage(slot, 0);
    }
}
