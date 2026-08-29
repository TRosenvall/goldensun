typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_22;
extern unsigned char Lc7c[] __asm__(".Lc7c");
extern unsigned char gScript_889__02008c64[];
extern int __GetFlag(int id);

void *OvlFunc_910_200809c(void)
{
    unsigned char *g;
    unsigned char *p;
    unsigned int k;
    unsigned int o;
    int v;
    int t;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_22)) {
        if (__GetFlag(0x84f) != 0) {
            p = Lc7c;
            t = 1;
            p += 0x76;
            *p = t;
        }
        if (__GetFlag(0x845) != 0) {
            p = Lc7c;
            t = 0;
            p += 0x46;
            *p = t;
        }
        return Lc7c;
    }
    return gScript_889__02008c64;
}
