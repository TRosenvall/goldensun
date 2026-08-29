extern unsigned char gState[];
extern int _AREA_8c;
extern int _AREA_8e;
extern unsigned char L3324[] __asm__(".L3324");
extern unsigned char L339c[] __asm__(".L339c");
extern unsigned char L35f4[] __asm__(".L35f4");
extern unsigned char L375c[] __asm__(".L375c");
extern unsigned char L37bc[] __asm__(".L37bc");
extern unsigned char L387c[] __asm__(".L387c");
extern unsigned char L399c[] __asm__(".L399c");
extern unsigned char L3a44[] __asm__(".L3a44");
extern unsigned char L3bdc[] __asm__(".L3bdc");
extern unsigned char L3e1c[] __asm__(".L3e1c");

extern int __GetFlag(int id);

unsigned char *OvlFunc_953_200807c(void)
{
    unsigned char *g;
    int a0;

    g = gState;
    a0 = *(short *)(g + (0xe0 << 1));
    if (a0 == (int)&_AREA_8c) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 5:
        case 69:
            return L339c;
        case 7:
        case 70:
            return L35f4;
        case 8:
        case 21:
        case 31:
        case 64:
        case 65:
        case 67:
            return L37bc;
        case 12:
            return L387c;
        case 66:
        case 68:
            return L399c;
        }
        return L375c;
    }
    if (a0 == (int)&_AREA_8e) {
        if (__GetFlag(0x95 << 4))
            return L3e1c;
        if (__GetFlag(0x962))
            return L3bdc;
        return L3a44;
    }
    return L3324;
}
