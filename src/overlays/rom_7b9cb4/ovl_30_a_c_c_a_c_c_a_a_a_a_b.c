typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int __GetFlag(int id);
extern int _AREA_4d;
extern int _AREA_4e;
extern int _AREA_4f;
extern int _AREA_50;
extern int _AREA_51;
extern int _AREA_52;
extern int _AREA_53;
extern int _AREA_54;
extern int _AREA_55;
extern int _AREA_56;
extern int _AREA_57;
extern unsigned char gScript_882__0200cd6c[];
extern unsigned char gScript_882__0200ce5c[];
extern unsigned char gScript_881__0200cebc[];
extern unsigned char L4d18[] __asm__(".L4d18");
extern unsigned char L4d24[] __asm__(".L4d24");
extern unsigned char L4d9c[] __asm__(".L4d9c");
extern unsigned char L4dc0[] __asm__(".L4dc0");
extern unsigned char L4f34[] __asm__(".L4f34");
extern unsigned char L4fb8[] __asm__(".L4fb8");
extern unsigned char L506c[] __asm__(".L506c");
extern unsigned char L50cc[] __asm__(".L50cc");
extern unsigned char L512c[] __asm__(".L512c");
extern unsigned char L5150[] __asm__(".L5150");

unsigned char *OvlFunc_932_2009678(void)
{
    unsigned int base;
    unsigned int off;
    short v;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_AREA_4d)) {
        if (__GetFlag(0x8fd))
            return gScript_882__0200cd6c;
        return L4d24;
    }
    if (v == (int)(&_AREA_4e))
        return L4d9c;
    if (v == (int)(&_AREA_4f))
        return L4dc0;
    if (v == (int)(&_AREA_50))
        return gScript_882__0200ce5c;
    if (v == (int)(&_AREA_51))
        return gScript_881__0200cebc;
    if (v == (int)(&_AREA_52))
        return L4f34;
    if (v == (int)(&_AREA_53))
        return L4fb8;
    if (v == (int)(&_AREA_54))
        return L506c;
    if (v == (int)(&_AREA_55))
        return L50cc;
    if (v == (int)(&_AREA_56))
        return L512c;
    if (v == (int)(&_AREA_57))
        return L5150;
    return L4d18;
}
