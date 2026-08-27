extern char *iwram_3001ebc;
extern unsigned char L1778[] __asm__(".L1778");
extern unsigned char L178e[] __asm__(".L178e");
extern unsigned char L17a4[] __asm__(".L17a4");
extern unsigned char L17ba[] __asm__(".L17ba");
extern unsigned char L17d0[] __asm__(".L17d0");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void OvlFunc_928_2008de8(int n);

void OvlFunc_928_2008e4c(void)
{
    char *p;

    p = iwram_3001ebc;
    __CutsceneStart();
    switch (*(short *)(p + (0xb6 << 1))) {
    case 1:
        __PlaySound(0x9e);
        __Func_8010560(L1778, 0x51, 0x12);
        break;
    case 2:
        __PlaySound(0x9e);
        __Func_8010560(L178e, 0x53, 0xb);
        break;
    case 3:
        __PlaySound(0x9e);
        __Func_8010560(L178e, 0x56, 0xb);
        break;
    case 4:
        __PlaySound(0x9e);
        __Func_8010560(L17a4, 0x54, 0x18);
        break;
    case 5:
        __PlaySound(0x9e);
        __Func_8010560(L17a4, 0x48, 7);
        break;
    case 6:
        __PlaySound(0xbc);
        __Func_8010560(L17ba, 0x45, 0xb);
        break;
    case 7:
        __PlaySound(0x9e);
        __Func_8010560(L17d0, 0x53, 7);
        break;
    }
    OvlFunc_928_2008de8(*(short *)(p + (0xb6 << 1)));
    __CutsceneEnd();
}
