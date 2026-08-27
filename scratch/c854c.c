extern unsigned char gState[];
extern unsigned char L7420[] __asm__(".L7420");
extern unsigned char L7444[] __asm__(".L7444");
extern unsigned char L7570[] __asm__(".L7570");
extern unsigned char L76fc[] __asm__(".L76fc");
extern unsigned char L781c[] __asm__(".L781c");
extern unsigned char L7930[] __asm__(".L7930");
extern unsigned char L7984[] __asm__(".L7984");
extern unsigned char L79c0[] __asm__(".L79c0");
extern unsigned char L7b58[] __asm__(".L7b58");
extern unsigned char L7d44[] __asm__(".L7d44");
extern unsigned char L7edc[] __asm__(".L7edc");

extern int __GetFlag(int id);

unsigned char *OvlFunc_945_200854c(void)
{
    unsigned char *g;

    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
    case 2:
        if (__GetFlag(0x8a << 4))
            return L76fc;
        if (__GetFlag(0x928) && __GetFlag(0x93e) == 0)
            return L7570;
        return L7444;
    case 4:
    case 23:
        if (__GetFlag(0x93e))
            return L7edc;
        return L79c0;
    case 5:
        if (__GetFlag(0x8a << 4))
            return L7930;
        if (__GetFlag(0x93e))
            return L7984;
        return L781c;
    case 15:
    case 17:
    case 19:
        return L7b58;
    case 21:
        return L7d44;
    }
    return L7420;
}
