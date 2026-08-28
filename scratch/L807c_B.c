extern unsigned int gState;
extern int _AREA_4b;
extern int _AREA_4c;
extern unsigned char L140c[] __asm__(".L140c");
extern unsigned char L15bc[] __asm__(".L15bc");
extern unsigned char L13f4[] __asm__(".L13f4");
extern int __GetFlag(int id);
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_931_200807c(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int g;
    unsigned int off;
    int v;
    int z;

    g = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    g += off;
    v = *(short *)g;
    if (v == (int)(&_AREA_4b)) {
        if (__GetFlag(0x909) != 0) {
            z = 0;
            p = L140c;
            q = L140c;
            p += 0x8e;
            q += 0xa6;
            *p = z;
            *q = z;
        }
        return L140c;
    }
    if (v == (int)(&_AREA_4c)) {
        if (__GetFlag(0x8fd) != 0) {
            p = L15bc;
            p += 0x2e;
            z = 1;
            *p = z;
        }
        if (__GetFlag(0x8fe) != 0 || __GetFlag(0x907) != 0) {
            p = L15bc;
            p += 0x5e;
            z = 1;
            *p = z;
        }
        q = L15bc;
        __Func_808b868(q);
        return q;
    }
    return L13f4;
}
