struct Actor {
    unsigned char pad0[6];
    unsigned short f6;
    unsigned char pad8[0x51];
    unsigned char f59;
};

extern int __GetFlag(int id);
extern int __MapActor_SetPos(int slot, int x, int z);
extern struct Actor *__MapActor_GetActor(int slot);

void OvlFunc_931_2008b2c(void)
{
    struct Actor *a;
    int v;

    if (!__GetFlag(0x240)) {
        __MapActor_SetPos(8, 0x3280000, 0x2d70000);
        __MapActor_GetActor(8)->f6 = 0x3000;
        __MapActor_SetPos(9, 0x31a0000, 0x3390000);
    }
    if (!__GetFlag(0x241)) {
        __MapActor_SetPos(0xa, 0x2300000, 0x2c60000);
        __MapActor_GetActor(0xa)->f6 = 0x1000;
        __MapActor_SetPos(0xb, 0x2400000, 0x2c50000);
    }
    if (!__GetFlag(0x242)) {
        __MapActor_SetPos(0xf, 0x1270000, 0x2e80000);
        a = __MapActor_GetActor(0xf);
        v = 0xb000;
        a->f6 = v;
    } else {
        a = __MapActor_GetActor(0xf);
        a->f59 |= 4;
    }
    a = __MapActor_GetActor(0x11);
    if (a != 0)
        a->f59 |= 4;
    a = __MapActor_GetActor(0x10);
    if (a != 0)
        a->f59 |= 4;
}
