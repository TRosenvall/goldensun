typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char L5cc8[] __asm__(".L5cc8");
extern unsigned char L5ab8[] __asm__(".L5ab8");
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_899_2008048(void)
{
    unsigned int base;
    unsigned int off;
    short v;
    unsigned char *p;

    base = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    switch (v) {
    case 0xf: case 0x10: case 0x11:
        p = L5cc8;
        break;
    default:
        p = L5ab8;
        break;
    }
    __Func_808b868(p);
    return p;
}
