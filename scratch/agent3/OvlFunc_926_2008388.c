typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_3c;
extern int __GetFlag(int id);
extern void __Func_808b868(unsigned char *p);
extern unsigned char L48f0[] __asm__(".L48f0");
extern unsigned char L4ae8[] __asm__(".L4ae8");
extern unsigned char L4998[] __asm__(".L4998");

unsigned char *OvlFunc_926_2008388(void)
{
    unsigned int base;
    unsigned int off;
    unsigned int p;
    unsigned char *q;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    p = base + off;
    off = 0;
    if (*(short *)((char *)p + off) == (int)(&_AREA_3c))
        return L48f0;
    off = 0xe1;
    off <<= 1;
    p = base + off;
    off = 0;
    if (*(short *)((char *)p + off) == 3)
        return L4ae8;
    if (__GetFlag(0x895) != 0) {
        q = L4998;
        *(unsigned short *)(q + 0x7a) = 0x895;
        *(unsigned short *)(q + 0xaa) = 0x895;
        *(int *)(q + 0xc8) = 0x1200000;
        *(int *)(q + 0xd0) = 0xf80000;
        *(unsigned short *)(q + 0x10a) = 0x895;
        *(unsigned short *)(q + 0x122) = 0x895;
    }
    __Func_808b868(L4998);
    return L4998;
}
