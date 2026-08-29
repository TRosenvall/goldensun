extern char *iwram_3001ebc;
extern short L1d0c[] __asm__(".L1d0c");
extern void *L1cf0[] __asm__(".L1cf0");

extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8091e9c(int n);

void OvlFunc_907_2008328(void)
{
    char *w;
    unsigned char *p;
    unsigned int i;
    int k;
    int z;

    w = iwram_3001ebc;
    z = 0;
    __CutsceneStart();
    for (i = 8; i <= 0x41; i++) {
        p = __MapActor_GetActor(i);
        if (p != 0)
            p[0x55] = z;
    }
    k = (short)(*(unsigned short *)(w + (0xb6 << 1)) - 3);
    if (k == 6)
        __PlaySound(0xbc);
    else
        __PlaySound(0x9e);
    __Func_8010560(L1cf0[k], L1d0c[k * 2], L1d0c[k * 2 + 1]);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    p = __MapActor_GetActor(0);
    p[0x55] = 0;
    w = iwram_3001ebc;
    z = 0;
    *(int *)(w + (0xe0 << 1)) = 0x100;
    if (k == 6) {
        __MapActor_SetAnim(0, 2);
        __Func_809228c(0, 0, -4);
    } else {
        __Func_8092208(0, 3, -0x10);
    }
    if (k == 4)
        __Func_8092b08(0, 3);
    else
        __Func_8092b08(0, 2);
    __CutsceneWait(0x10);
    __Func_8091e9c(k + 3);
    __CutsceneEnd();
}
