struct Actor {
    unsigned char pad0[6];
    unsigned short facing;
    unsigned char pad1[0x51];
    unsigned char interactFlags;
};

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_931_2008b2c(void)
{
    struct Actor *a;

    if (__GetFlag(0x90 << 2) == 0) {
        __MapActor_SetPos(8, 0xca << 18, 0x2d70000);
        __MapActor_GetActor(8)->facing = 0xc0 << 6;
        __MapActor_SetPos(9, 0x31a0000, 0x3390000);
    }
    if (__GetFlag(0x241) == 0) {
        __MapActor_SetPos(0xa, 0x8c << 18, 0x2c60000);
        __MapActor_GetActor(0xa)->facing = 0x80 << 5;
        __MapActor_SetPos(0xb, 0x90 << 18, 0x2c60000);
    }
    if (__GetFlag(0x242) == 0) {
        __MapActor_SetPos(0xf, 0x1270000, 0xba << 18);
        __MapActor_GetActor(0xf)->facing = 0xb0 << 8;
    } else {
        __MapActor_GetActor(0xf)->interactFlags |= 4;
    }
    a = __MapActor_GetActor(0x11);
    if (a != 0)
        a->interactFlags |= 4;
    a = __MapActor_GetActor(0x10);
    if (a != 0)
        a->interactFlags |= 4;
}
