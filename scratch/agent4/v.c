struct E1 {
    int f0;
    short f4;
    short f6;
    void (*f8)(void);
};

struct E2 {
    short f0;
    short f2;
    int f4;
    int f8;
    int fc;
    int f10;
    short f14;
    short f16;
};

extern struct E1 gOvl_0200e3f4[];
extern struct E2 gTbl2[] __asm__(".L5b84");
extern void OvlFunc_881_200a858(void);

void OvlFunc_881_200a768(void)
void OvlFunc_881_200a768(void)
{
    int i;

    i = 0;
loop:
    if (gOvl_0200e3f4[i].f0 == 1 && gOvl_0200e3f4[i].f4 == 0x8a) {
        gOvl_0200e3f4[i].f0 = 2;
        gOvl_0200e3f4[i].f8 = OvlFunc_881_200a858;
    }
    if (gOvl_0200e3f4[i].f0 != -1) {
        i++;
        goto loop;
    }
    for (i = 0; ; i++) {
        if (gTbl2[i].f0 == 0x39) {
            gTbl2[i].f8 = 0x17940000;
            gTbl2[i].f10 = 0xd480000;
            gTbl2[i].f14 = 0xc0 << 6;
            return;
        }
    }
}
