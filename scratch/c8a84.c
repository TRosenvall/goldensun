struct A {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x4c - 0x20];
    int f4c;
    unsigned char pad50[0x55 - 0x50];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned short f66;
};

extern struct A *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);
extern unsigned char gScript_944__020093a4[];

void OvlFunc_944_2008a84(int slot)
{
    struct A *a;

    a = __MapActor_GetActor(slot);
    __Func_8092b08(slot, 1);
    a->f55 = 0;
    a->f64 = __Random() >> 15;
    a->f66 = __Random() >> 15;
    a->fc = ((unsigned short)(__Random() >> 14) << 16) + (0xc0 << 11);
    a->f4c = (unsigned short)(__Random() * 3 >> 3) + 0xffffd000;
    a->f18 = 0xa0 << 9;
    a->f1c = 0xa0 << 9;
    __MapActor_SetBehavior(slot, gScript_944__020093a4);
}
