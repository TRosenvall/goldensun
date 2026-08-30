extern unsigned char gState[];
extern int _AREA_7e;
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void OvlFunc_946_2008e00(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_20092b4(void)
{
    unsigned char *e;
    unsigned char *f;
    unsigned char *gp;
    short *g;
    int base;
    int d;
    int r;
    int s2;

    e = __MapActor_GetActor(8);
    d = *(int *)(e + 8) >> 20;
    if (d == 0x28) {
        gp = gState;
        g = (short *)(gp + (0xe0 << 1));
        base = 0x8d2 - (int)&_AREA_7e;
        r = __GetFlag(*g + base);
        if (r == 0) {
            f = e + 0x55;
            *f = 3;
            __CutsceneWait(8);
            OvlFunc_946_2008e00(8);
            __PlaySound(0x88);
            __CutsceneWait(0x28);
            __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
            __Func_8092b08(8, 3);
            *f = r;
            e[0x23] |= 2;
            s2 = 0xa;
            __Func_8010704(0x2a, 0xa, 1, 1, d, s2);
            __SetFlag(*g + base);
        }
    }
}
