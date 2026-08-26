struct Act {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x18];
    int f44;
    int f48;
};

extern struct Act *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __Actor_SetSpriteFlags(struct Act *a, int f);
extern void __Func_8092158(int slot, int x, int z);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __WaitFrames(int n);

void OvlFunc_898_20091b0(int slot, int x, int z, int opt)
{
    struct Act *a;
    int i;

    a = __MapActor_GetActor(slot);
    __MapActor_SetSpeed(slot, 0xc0 << 10, 0xc0 << 9);
    a->f48 = 0x80 << 8;
    a->f44 = 0;
    a->f28 = opt;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092158(slot, x, z);
    __MapActor_SetPos(slot, x << 16, z << 16);
    for (i = 0x3c; i != 0; i--) {
        __WaitFrames(1);
        if (*(short *)((char *)a + 0x2a) == 0)
            break;
    }
    __Actor_SetSpriteFlags(a, 1);
    a->f48 = 0x80 << 9;
}
