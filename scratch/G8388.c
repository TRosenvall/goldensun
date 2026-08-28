extern unsigned char gState[];
extern int _AREA_3c;
extern unsigned char L48f0[] __asm__(".L48f0");
extern unsigned char L4ae8[] __asm__(".L4ae8");
extern unsigned char L4998[] __asm__(".L4998");
extern int __GetFlag(int id);
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_926_2008388(void)
{
    unsigned char *g;
    unsigned char *p;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_3c))
        return L48f0;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        return L4ae8;
    if (__GetFlag(0x895)) {
        p = L4998;
        *(short *)(p + 0x7a) = 0x895;
        *(short *)(p + 0xaa) = 0x895;
        *(int *)(p + 0xc8) = 0x90 << 17;
        *(int *)(p + 0xd0) = 0xf8 << 16;
        *(short *)(p + (0x85 << 1)) = 0x895;
        *(short *)(p + 0x122) = 0x895;
    }
    __Func_808b868(L4998);
    return L4998;
}
