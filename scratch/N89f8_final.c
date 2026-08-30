extern unsigned char gState[];
extern int ewram_2001000;
extern unsigned char *iwram_3001ebc;

extern int __Func_8077348(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int C40I(int a, int b) __asm__("__Func_8092c40");
extern void __Func_808ba38(void);
extern void __Func_8019908(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __SetDestMap2(int a, int b);
extern void __Func_8091f90(int a, int b);
extern int _AREA_89;

void OvlFunc_951_20089f8(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned int v;
    unsigned int n;

    n = __Func_8077348() * 10;
    __CutsceneStart();
    g = gState;
    v = *(unsigned int *)(g + 0x10);
    if (v < n) {
        __MessageID(0xe12);
        __Func_8092c40(9, 0);
        return;
    }
    ewram_2001000 = v;
    __Func_808ba38();
    __MessageID(0xe0e);
    __Func_8019908(n, 5);
    C40I(9, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __ActorMessage(9, 0);
        __Func_80921c4(0, 0x78, 0x80);
        __Func_80921c4(0, 0x78, 0x98);
        __Func_8092adc(0, 0x80 << 8, 0);
        __CutsceneWait(0x14);
        __SetDestMap2(0x1fd, 0);
        __Func_8091f90((int)&_AREA_89, 0xd);
    } else {
        p = iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
        __ActorMessage(9, 0);
    }
    __CutsceneEnd();
}
