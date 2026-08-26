struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x55 - 0x14];
    unsigned char f55;
};

extern unsigned char L8[] __asm__(".L8");
extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __Actor_SetScript(struct A *a, unsigned char *s);
extern void __PlaySound(int id);

int OvlFunc_common1_17c0(struct A *a)
{
    short *p;
    struct A *b;
    int zero;

    p = (short *)((char *)a + 0x64);
    b = __MapActor_GetActor(*p);
    __Actor_TravelTo(b, a->f8, a->fc + (0x90 << 14), a->f10);
    zero = 0;
    b->f55 = zero;
    __Actor_SetScript(b, L8);
    __PlaySound(0x53);
    *p = zero;
    return zero;
}
