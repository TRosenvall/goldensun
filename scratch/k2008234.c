struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __UI_Sanctum(int slot);

void OvlFunc_967_2008234(int slot)
{
    unsigned short d;

    d = (__MapActor_GetActor(0)->facing + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __UI_Sanctum(slot);
    } else if (__GetFlag(0x9a7)) {
        __MessageID(0x28fc);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x26f6);
        __ActorMessage(slot, 0);
    }
}
