extern unsigned int iwram_3001e40;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __MapActor_SetBehavior(int a, void *script);
extern unsigned char gScript_886__02009440[];

void OvlFunc_886_20090c0(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *q;
    int slot;
    unsigned int t;
    int h;

    t = iwram_3001e40 % 0xb4;
    slot = 0x17;
    switch (t) {
    case 0xa:
        break;
    case 0x14:
        slot = 0x18;
        break;
    case 0x1e:
        slot = 0x19;
        break;
    default:
        return;
    }
    a = __MapActor_GetActor(slot);
    if (a == 0) {
        return;
    }
    b = __MapActor_GetActor(8);
    if (b != 0) {
        __MapActor_SetPos(slot, *(int *)(b + 8), *(int *)(b + 0x10));
    }
    *(int *)(a + 0x18) = 0x6666;
    *(int *)(a + 0x1c) = 0x6666;
    *(int *)(a + 0xc) = *(int *)(a + 0xc) + (0xc0 << 13);
    q = a + 0x64;
    *(int *)(a + 0x3c) = *(int *)(a + 0xc);
    h = 0x19;
    *(short *)q = h;
    q += 2;
    h = 0x80;
    *(short *)q = h;
    __MapActor_SetBehavior(slot, gScript_886__02009440);
}
