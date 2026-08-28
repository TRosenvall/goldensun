typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_35;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091f90(int id, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_923_20091b4(void)
{
    unsigned char *base;
    unsigned int off;
    unsigned char *g;

    __CutsceneStart();
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    off = 0xe0;
    off <<= 1;
    base = (unsigned char *)iwram_3001ebc + off;
    off += 0x40;
    *(unsigned int *)base = off;
    __Func_8091f90((int)(&_AREA_35), 0x1f);
    g = (unsigned char *)&gState;
    g = g + 0x22b;
    *g = 3;
    __Func_8091eb0(0x24, 1);
    __CutsceneEnd();
}
