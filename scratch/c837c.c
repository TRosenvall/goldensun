extern unsigned char gState[];
extern unsigned char L5b84[] __asm__(".L5b84");
extern unsigned char L604c[] __asm__(".L604c");
extern unsigned char L6154[] __asm__(".L6154");
extern unsigned char L61e4[] __asm__(".L61e4");
extern unsigned char L625c[] __asm__(".L625c");
extern unsigned char L628c[] __asm__(".L628c");
extern unsigned char L62ec[] __asm__(".L62ec");
extern unsigned char L6394[] __asm__(".L6394");
extern unsigned char L63c4[] __asm__(".L63c4");

extern int __GetFlag(int id);
extern void __SetFlag(int id);

unsigned char *OvlFunc_881_200837c(void)
{
    unsigned char *g;

    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 0x31:
        if (__GetFlag(0x94f))
            break;
        if (__GetFlag(0x941) == 0)
            break;
        return L6154;
    case 0x40:
        if (__GetFlag(0x85a))
            break;
        return L604c;
    case 0x41:
    case 0x46:
        return L61e4;
    case 0x47:
        return L628c;
    case 0x48:
        return L6394;
    case 0x49:
        return L63c4;
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x4b:
        return L625c;
    case 0x50:
        return L62ec;
    }
    __SetFlag(0x235);
    return L5b84;
}
