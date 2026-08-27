#define REG_BLDCNT   (*(volatile unsigned short *)0x04000050)
#define REG_BLDALPHA (*(volatile unsigned short *)0x04000052)

extern unsigned char iwram_3001e70[];
extern unsigned short L1fc0[] __asm__(".L1fc0");
extern void __PlaySound(int id);
extern void __WaitFrames(int n);

void OvlFunc_951_2008880(void)
{
    unsigned char *p;
    unsigned short *t;
    unsigned short mode;
    int i;

    p = *(unsigned char **)iwram_3001e70;
    __PlaySound(0xd8);
    p += 0x164;
    for (i = 15; i >= 0; i--) {
        *(int *)(p + 0xc) += 0xffff0000;
        __WaitFrames(4);
    }
    mode = 0x3f42;
    t = L1fc0;
    i = 7;
    do {
        REG_BLDCNT = mode;
        REG_BLDALPHA = *t;
        i--;
        t++;
        __WaitFrames(8);
    } while (i >= 0);
}
