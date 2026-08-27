extern unsigned char gState[];
extern int _AREA_7e;
extern unsigned char gOvl_0200b2bc[];
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_946_20093ac(void)
{
    unsigned char *g;
    short *e;

    g = gState;
    e = (short *)(g + (0xe0 << 1));
    if (__GetFlag(*e + (0x8c8 - (int)&_AREA_7e)) == 0) {
        __CutsceneStart();
        __Func_8091f90(*e, 5);
        g[0x22b] = 3;
        switch (*e - (int)&_AREA_7e) {
        case 0: __Func_8091eb0(0x3f, 0); break;
        case 1: __Func_8091eb0(0x3f, 1); break;
        case 2: __Func_8091eb0(0x3f, 2); break;
        case 3: __Func_8091eb0(0x3f, 3); break;
        case 4: __Func_8091eb0(0x54, 0); break;
        case 5: __Func_8091eb0(0x54, 1); break;
        case 6: __Func_8091eb0(0x54, 2); break;
        case 7: __Func_8091eb0(0x54, 3); break;
        case 8: __Func_8091eb0(0x54, 4); break;
        }
        __CutsceneEnd();
    } else {
        __Func_8010560(gOvl_0200b2bc, 0x2c, 7);
        __PlaySound(0xb7);
        __Func_8091e9c(3);
    }
}
