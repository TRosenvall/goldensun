extern unsigned char gState[];
extern int _AREA_64;
extern int _AREA_65;
extern unsigned char L8d4[] __asm__(".L8d4");
extern unsigned char La0c[] __asm__(".La0c");
extern unsigned char L784[] __asm__(".L784");
extern unsigned char gScript_906__0200879c[];
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_937_200807c(void)
{
    unsigned char *g;
    unsigned char *r;
    int e;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_64)) {
        e = *(short *)(g + (0xe1 << 1));
        switch (e) {
        case 9: case 0xa: case 0xb: case 0xc:
        case 0xd: case 0xe: case 0xf: case 0x11:
            r = L8d4;
            break;
        default:
            r = gScript_906__0200879c;
            break;
        }
        __Func_808b868(r);
        return r;
    }
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_65))
        return La0c;
    return L784;
}
