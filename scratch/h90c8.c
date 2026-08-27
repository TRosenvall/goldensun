extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int gOvl_020098ec;
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int n);

void OvlFunc_905_20090c8(void)
{
    char *p;
    unsigned char *g;
    int n;
    int v;

    p = iwram_3001ebc;
    v = 0x80 << 6;
    n = ++gOvl_020098ec;
    switch (n) {
    case 0x3c:
        __Func_8092adc(0xd, v, 0);
        __MapActor_Emote(0xd, 2, 0);
        break;
    case 0xb4:
        __Func_809259c(0xd, 3);
        break;
    case 0xf0:
    case 0x10e:
        __MapActor_Jump(0xd, 4, 0);
        break;
    case 0x1e0:
        __MapActor_SetAnim(0xd, 4);
        break;
    }
    g = gState;
    if (*(short *)(g + (0x8d << 2)) == 0)
        *(short *)(p + (0xc1 << 1)) = 0x63;
}
