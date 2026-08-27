struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x55 - 0x14];
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_883_200b45c(int first, unsigned int n, int mode)
{
    unsigned int i;
    int slot;
    struct Actor *a;

    slot = first;
    if (mode == 0) {
        for (i = 0; i < n; i++) {
            a = __MapActor_GetActor(slot);
            a->f55 = 0;
            __Actor_SetSpriteFlags(a, 0);
            a->f8 = 0xc3 << 17;
            a->fc = 0xa0 << 16;
            a->f10 = 0x34a0000;
            slot++;
        }
    } else {
        for (i = 0; i < n; i++) {
            __MapActor_SetPos(slot, 0, 0);
            slot++;
        }
    }
}
