extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_907_2008f3c(void *actor);
extern short L1d88[] __asm__(".L1d88");

void OvlFunc_907_2008ed8(void)
{
    unsigned char *gs;
    unsigned char *a;
    int lim;
    int lim2;
    int wrap;

    lim = 0x8e << 16;
    lim2 = 0x80 << 12;
    wrap = 0xf0 << 13;
    gs = gState;
    a = __MapActor_GetActor(*(int *)(gs + 0x1f4));
    if (*(int *)(a + 8) >= lim)
        return;
    if (*(int *)(a + 0xc) < lim2) {
        if (L1d88[0] == 0)
            OvlFunc_907_2008f3c(a);
        L1d88[0]++;
        if (L1d88[0] == (wrap >> 16))
            L1d88[0] = 0;
    } else {
        L1d88[0] = 0;
    }
}
