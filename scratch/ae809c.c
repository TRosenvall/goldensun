extern unsigned char gState[];
extern int _AREA_21;
extern unsigned char L29b4[] __asm__(".L29b4");
extern unsigned char L299c[] __asm__(".L299c");
extern void __Func_808b868(unsigned char *p);
extern int __GetFlag(int id);

unsigned char *OvlFunc_909_200809c(void)
{
    unsigned char *gp;
    unsigned char *p;

    gp = gState;
    if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_21) {
        p = L29b4;
        __Func_808b868(p);
        if (__GetFlag(0x84e)) {
            p[0xa6] = 2;
            p[0xbe] = 0;
            p[0xd6] = 3;
            p[0xee] = 1;
        }
        return p;
    } else {
        return L299c;
    }
}
