extern unsigned int gState;
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);

void OvlFunc_881_20084a0(int slot, int a, int b)
{
    unsigned char *e1;
    unsigned char *e2;
    unsigned char *base;
    unsigned int r2;
    unsigned int r3;
    short *p;

    e1 = __MapActor_GetActor(slot - 0x64);
    r3 = (unsigned int)&gState;
    r2 = 0xfa;
    r2 <<= 1;
    r3 += r2;
    e2 = __MapActor_GetActor(*(int *)r3);
    base = iwram_3001ebc;
    if (*(int *)(e2 + 8) < *(int *)(e1 + 8)) {
        r2 = 0xb8;
        r2 <<= 1;
        p = (short *)(base + r2);
        *p = a;
    } else {
        r2 = 0xb8;
        r2 <<= 1;
        p = (short *)(base + r2);
        *p = b;
    }
    __PlaySound(0x7b);
}

void OvlFunc_881_20084f0(int slot, int a, int b)
{
    unsigned char *e1;
    unsigned char *e2;
    unsigned char *base;
    unsigned int r2;
    unsigned int r3;
    short *p;

    e1 = __MapActor_GetActor(slot - 0x64);
    r3 = (unsigned int)&gState;
    r2 = 0xfa;
    r2 <<= 1;
    r3 += r2;
    e2 = __MapActor_GetActor(*(int *)r3);
    base = iwram_3001ebc;
    if (*(int *)(e2 + 0x10) < *(int *)(e1 + 0x10)) {
        r2 = 0xb8;
        r2 <<= 1;
        p = (short *)(base + r2);
        *p = a;
    } else {
        r2 = 0xb8;
        r2 <<= 1;
        p = (short *)(base + r2);
        *p = b;
    }
    __PlaySound(0x7b);
}
