extern unsigned char *iwram_3001ebc;
extern void *L3190[] __asm__(".L3190");
extern short L31a8[][2] __asm__(".L31a8");

extern void __CutsceneStart(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Func_8010560(void *p, int a, int b);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092b08(int slot, int a);
extern void __Func_809228c(int slot, int a, int b);
extern void __Func_8092208(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __CutsceneEnd(void);

void OvlFunc_921_20086c0(void)
{
    unsigned char *base;
    unsigned char *a;
    unsigned int i;
    int n, t, b1, b2, k1, k2, k3, k4, k5;

    base = iwram_3001ebc;
    __CutsceneStart();
    for (i = 8; i <= 0x41; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0)
            a[0x55] = 0;
    }
    t = *(unsigned short *)(base + 0x16c);
    n = (short)(t - 0x32);
    k1 = 0x80 << 8;
    k5 = 0x80 << 7;
    k2 = 0x3333;
    k3 = -8;
    k4 = -0x10;
    if (n == 6)
        __PlaySound(0xbc);
    else
        __PlaySound(0x9e);
    b1 = L31a8[n - 1][0];
    b2 = L31a8[n - 1][1];
    __Func_8010560(L3190[n - 1], b1, b2);
    __MapActor_SetSpeed(0, k1, k5);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x100;
    if (n == 6) {
        __MapActor_SetSpeed(0, k2, 0x1999);
        __MapActor_SetAnim(0, 2);
        __Func_8092b08(0, 3);
        __Func_809228c(0, 0, k3);
    } else {
        a = __MapActor_GetActor(0);
        a += 0x55;
        *a = 0;
        __Func_8092208(0, 3, k4);
    }
    __CutsceneWait(0x10);
    __Func_8091e9c(n);
    __CutsceneEnd();
}
