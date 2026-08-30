extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_35;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_923_20091b4(void)
{
    unsigned char *p;
    unsigned char *gp;

    __CutsceneStart();
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = (0xe0 << 1) + 0x40;
    __Func_8091f90((int)&_AREA_35, 0x1f);
    gp = gState + 0x200;
    gp[0x2b] = 3;
    __Func_8091eb0(0x24, 1);
    __CutsceneEnd();
}
