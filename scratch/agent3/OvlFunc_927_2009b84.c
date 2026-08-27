extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2009b84(void)
{
    unsigned char *a;
    unsigned char *p;
    int x;
    int z;
    int t;
    int one;
    int zero;

    __CutsceneStart();
    x = *(int *)(__MapActor_GetActor(0xd) + 8);
    t = *(int *)(__MapActor_GetActor(0xd) + 0x10);
    x >>= 20;
    z = t >> 20;
    one = 1;
    OvlFunc_927_2008244(2, x, z, 1, one, 0xff);
    zero = 0;
    OvlFunc_927_2008244(2, x + 1, z, 1, one, zero);
    OvlFunc_927_2008244(2, x - 1, z, 1, one, zero);
    OvlFunc_927_2008244(2, x, z + 1, 1, one, zero);
    OvlFunc_927_2008244(2, x, z - 1, 1, one, zero);
    if (x == 0x2d && z == 6) {
        a = __MapActor_GetActor(0xd);
        p = a + 0x55;
        *p = zero;
        *(int *)(a + 0x14) = 0xfffe0000;
        *(int *)(a + 0xc) = 0xfffe0000;
    }
    __CutsceneEnd();
}
