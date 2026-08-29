extern unsigned char gState[];
extern unsigned char L3e34[] __asm__(".L3e34");
extern unsigned char L3e70[] __asm__(".L3e70");
extern unsigned char L3ec4[] __asm__(".L3ec4");
extern unsigned char L3f0c[] __asm__(".L3f0c");
extern unsigned char L3f78[] __asm__(".L3f78");
extern unsigned char L3fd8[] __asm__(".L3fd8");
extern unsigned char L4038[] __asm__(".L4038");
extern unsigned char L4080[] __asm__(".L4080");
extern unsigned char L40ec[] __asm__(".L40ec");

extern int __GetFlag(int id);

unsigned char *OvlFunc_888_200814c(void)
{
    unsigned char *g;
    int area;

    g = gState;
    area = *(short *)(g + (0xe1 << 1));
    switch (area) {
    case 0xa:
    case 0xc:
        return L3e70;
    case 0xb:
        return L3ec4;
    case 0x14:
    case 0x15:
    case 0x32:
        return L3f0c;
    case 0x20:
        return L40ec;
    case 0x1d:
        return L4038;
    case 0x23:
        return L4080;
    }
    if (__GetFlag(0x87a))
        return L3fd8;
    if (__GetFlag(0x815))
        return L3f78;
    return L3e34;
}
