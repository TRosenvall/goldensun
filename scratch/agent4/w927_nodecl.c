struct Actor {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x44 - 0x2c];
    int f44;
    int f48;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int slot, int a);

extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_8092158(int slot, int x, int y);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_927_2008d90(int slot, int x, int y, int z)
{
    struct Actor *a;

    a = __MapActor_GetActor(slot);
    __Func_8092b08(slot, 1);
    __MapActor_SetSpeed(slot, 0xc0 << 10, 0xc0 << 9);
    __PlaySound(0x98);
    a->f28 = z;
    a->f48 = 0x80 << 8;
    a->f44 = 0;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092158(slot, x, y);
    x <<= 16;
    y <<= 16;
    __MapActor_SetPos(slot, x, y);
    __Actor_SetSpriteFlags(a, 1);
    a->f48 = 0x80 << 9;
}
