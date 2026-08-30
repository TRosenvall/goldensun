struct A {
    unsigned char pad00[6];
    short f6;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);
extern void SetPosD(int a, int b, int c) __asm__("__MapActor_SetPos");

void OvlFunc_943_20097a0(void)
{
    struct A *e;
    int h;
    int z;

    __MapActor_SetPos(0x15, 0x83 << 17, 0x2c20000);
    e = __MapActor_GetActor(0x15);
    h = 0xa0 << 7;
    e->f6 = h;
    __MapActor_SetPos(0x18, 0xa4 << 16, 0xa2 << 18);
    e = __MapActor_GetActor(0x18);
    z = 0;
    e->f6 = z;
    __Func_8092b08(0x18, 1);
    __MapActor_SetPos(0x19, 0xc6 << 16, 0x2990000);
    e = __MapActor_GetActor(0x19);
    e->f6 = 0x80 << 8;
    __Func_8092b08(0x19, 1);
    __MapActor_SetPos(0x1a, 0xbc << 16, 0x2a60000);
    e = __MapActor_GetActor(0x1a);
    e->f6 = 0xb0 << 8;
    __MapActor_SetPos(0x1b, 0xba << 16, 0x27b0000);
    e = __MapActor_GetActor(0x1b);
    e->f6 = h;
    __MapActor_SetPos(0x16, 0, 0);
    SetPosD(0x17, 0, 0);
    SetPosD(0x14, 0, 0);
}
