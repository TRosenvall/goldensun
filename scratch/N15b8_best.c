struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x30 - 0x10];
    int f30;
    int f34;
    unsigned char pad38[0x5b - 0x38];
    unsigned char f5b;
};

extern struct Actor *__GetFieldActor(int slot);
extern void __Actor_Stop(struct Actor *a);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int z, int y);
extern void __Actor_WaitMovement(struct Actor *a);

void OvlFunc_common1_15b8(int slot, int x, int y)
{
    struct Actor *a;
    int v;

    a = __GetFieldActor(slot);
    if (a != 0) {
        v = 0xa0 << 9;
        a->f30 = v;
        a->f34 = v >> 1;
        a->f5b = 0;
        __Actor_Stop(a);
        __Actor_SetAnim(a, 5);
        __Actor_TravelTo(a, x << 16, a->fc, y << 16);
        __Actor_WaitMovement(a);
        __Actor_SetAnim(a, 1);
    }
}
