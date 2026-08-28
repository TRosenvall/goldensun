extern unsigned char gState[];
extern int _AREA_10;
extern int _AREA_13;
extern unsigned char L1fd8[] __asm__(".L1fd8");
extern unsigned char L2050[] __asm__(".L2050");
extern unsigned char L21b8[] __asm__(".L21b8");
extern unsigned char L22a8[] __asm__(".L22a8");
extern unsigned char L1fc0[] __asm__(".L1fc0");
extern unsigned char L22e4[] __asm__(".L22e4");
extern unsigned char L232c[] __asm__(".L232c");
extern unsigned char L241c[] __asm__(".L241c");
extern unsigned char L2524[] __asm__(".L2524");
extern unsigned char L22d8[] __asm__(".L22d8");
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_895_200807c(void)
{
    unsigned char *g;
    unsigned char *r;
    int e;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_10)) {
        e = *(short *)(g + (0xe1 << 1));
        switch (e) {
        case 0xe: case 0xf: case 0x10:
            return L21b8;
        case 0xb: case 0xc: case 0xd:
            return L2050;
        default:
            r = L1fd8;
            __Func_808b868(r);
            return r;
        }
    }
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_13))
        return L22a8;
    return L1fc0;
}

unsigned char *OvlFunc_895_20080ec(void)
{
    unsigned char *g;
    int e;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_13))
        return L22e4;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_10)) {
        e = *(short *)(g + (0xe1 << 1));
        switch (e) {
        case 0xb: case 0xc: case 0xd:
            return L241c;
        case 0xe: case 0xf: case 0x10:
            return L2524;
        default:
            return L232c;
        }
    }
    return L22d8;
}
