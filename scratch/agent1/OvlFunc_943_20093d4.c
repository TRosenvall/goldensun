extern unsigned char iwram_3001e70[];

extern unsigned int L5b38 __asm__(".L5b38");
extern unsigned int L5b50[] __asm__(".L5b50");
extern unsigned int L5b58 __asm__(".L5b58");
extern unsigned int L5b60 __asm__(".L5b60");

extern void __ClearFlag(int flag);
extern int __Random(void);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern void OvlFunc_943_2009444(void);

int OvlFunc_943_20093d4(void)
{
    unsigned char *p;

    p = *(unsigned char **)iwram_3001e70 + 0x104;
    __ClearFlag(0x11c);
    *(int *)(*(unsigned char **)(iwram_3001e70 + 0x4c) + 0x1c0) = 0x209;
    *(int *)(p + 0x1c) = 0;
    L5b58 = (unsigned short)__Random();
    L5b38 = (unsigned short)__Random();
    L5b50[0] = 0;
    L5b50[1] = 0;
    L5b60 = 0;
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_943_2009444();
    return 0;
}
