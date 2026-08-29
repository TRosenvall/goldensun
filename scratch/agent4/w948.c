struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern unsigned char gState[];
extern int L2f74[] __asm__(".L2f74");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int vx, int vy);
extern void __PlaySound(int id);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_948_200952c(int idx)
{
    struct Actor *a;
    unsigned char *gs;
    int gx;
    int gy;
    int slot;

    a = __MapActor_GetActor(0);
    gx = a->f8 / 0x100000;
    gy = a->f10 / 0x100000;
    gs = gState;
    slot = idx + 0xa;
    if (*(short *)(gs + 0x24a) != slot && gx != L2f74[idx]) {
        __MapActor_SetSpeed(slot, 0x90 << 11, 0x90 << 10);
        __PlaySound(0xbc);
        __MapActor_TravelTo(slot, (gx << 4) + 8, 0xb4 << 1);
        L2f74[idx] = gx;
        if (gy <= 0x16) {
            __Func_809228c(0, 0, 8);
        }
        __MapActor_WaitMovement(0);
    }
}
