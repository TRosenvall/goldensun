extern char *iwram_3001e9c;
extern void CloseUIBox(int a, int b);
extern void WaitFrames(int n);
extern int Func_8017394(int a);
extern void Func_8003f3c(int n);
extern void gfree(int n);

void Func_801c954(void)
{
    char *s;
    int *w;
    int off;

    s = iwram_3001e9c;
    off = 0xff4;
    CloseUIBox(*(int *)(s + off), 0);
    off = 0xff4;
    w = (int *)(s + off);
    while (Func_8017394(*w) == 0)
        WaitFrames(1);
    if (*(unsigned short *)(s + 0x46) != 0)
        Func_8003f3c(*(unsigned short *)(s + 0x48));
    off = 0x352;
    if (*(unsigned short *)(s + off) != 0) {
        off += 2;
        Func_8003f3c(*(unsigned short *)(s + off));
    }
    gfree(0x13);
}
