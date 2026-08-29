extern char *iwram_3001ebc;
extern unsigned char L2e48[] __asm__(".L2e48");
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void OvlFunc_911_20082b4(int n);

void OvlFunc_911_2008304(void)
{
    char *p;
    char *q;
    int a;
    int b;
    int sx;
    int sy;

    p = iwram_3001ebc;
    a = 0;
    b = 0;
    sx = 0x80 << 8;
    sy = 0x80 << 7;
    __CutsceneStart();
    __PlaySound(0x9e);
    switch (*(short *)(p + (0xb6 << 1))) {
    case 5:
        a = 0x47;
        b = 9;
        break;
    case 6:
        a = 0x49;
        b = 0x11;
        break;
    case 7:
        a = 0x50;
        b = 0x15;
        break;
    case 8:
        a = 0x54;
        b = 0xc;
        break;
    case 9:
        q = __MapActor_GetActor(0);
        q[0x55] = 0;
        __MapActor_SetSpeed(0, sx, sy);
        __Func_809228c(0, 0, 8);
        *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
        __Func_8091e9c(9);
        __CutsceneEnd();
        return;
    }
    __Func_8010560(L2e48, a, b);
    OvlFunc_911_20082b4(*(short *)(p + (0xb6 << 1)));
    __CutsceneEnd();
}
