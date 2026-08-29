extern int iwram_3001e40;
extern unsigned short L67a0[] __asm__(".L67a0");
extern void __SetRegAnimDest(int dest, int val);

void OvlFunc_881_200b8fc(void)
{
    __SetRegAnimDest(0x4000050, 0x3f41);
    if ((iwram_3001e40 & 2) != 0)
        __SetRegAnimDest(0x4000052, 0xc | *L67a0);
    else
        __SetRegAnimDest(0x4000052, 0x10 | *L67a0);
}
