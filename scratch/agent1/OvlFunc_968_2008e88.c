extern unsigned char gState[];
extern int _AREA_b5;
extern int _AREA_b6;
extern int _AREA_b7;
extern int _AREA_b8;
extern int _AREA_b9;
extern int _AREA_ba;
extern unsigned char gScript_945__0200e904[];
extern unsigned char L68ec[] __asm__(".L68ec");
extern unsigned char L69c4[] __asm__(".L69c4");
extern unsigned char L6b74[] __asm__(".L6b74");
extern unsigned char L6c04[] __asm__(".L6c04");
extern unsigned char L6c64[] __asm__(".L6c64");
extern unsigned char L6cf4[] __asm__(".L6cf4");
extern void __Func_808b868(void *p);

unsigned char *OvlFunc_968_2008e88(void)
{
    unsigned char *gs;
    unsigned char *p;
    int v;

    gs = gState;
    v = *(short *)(gs + 0x1c0);
    if (v == (int)(&_AREA_b5))
        return gScript_945__0200e904;
    if (v == (int)(&_AREA_b6))
        p = L69c4;
    else if (v == (int)(&_AREA_b7))
        p = L6b74;
    else if (v == (int)(&_AREA_b8))
        p = L6c04;
    else if (v == (int)(&_AREA_b9))
        p = L6c64;
    else if (v == (int)(&_AREA_ba))
        p = L6cf4;
    else
        return L68ec;
    __Func_808b868(p);
    return p;
}
