extern unsigned char *iwram_3001ebc;
extern unsigned char L5d0[] __asm__(".L5d0");
extern unsigned char L5fe[] __asm__(".L5fe");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8091e9c(int a);

void OvlFunc_961_2008194(void)
{
    unsigned char *tbl;
    int idx;
    unsigned int off;
    unsigned short x;
    unsigned short y;

    idx = *(short *)(iwram_3001ebc + (0xb6 << 1));
    tbl = L5d0;
    off = idx << 2;
    x = *(short *)(tbl + off);
    off += 2;
    y = *(short *)(tbl + off);
    __PlaySound(0x9e);
    __Func_8010560(L5fe, x, y);
    __Func_80922c4(0, 0, -0x10);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(idx);
}
