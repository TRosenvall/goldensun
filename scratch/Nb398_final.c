extern unsigned char *iwram_3001e98;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern void Func_801b9ec(unsigned char *p, int n);
extern void WaitFrames(int n);
extern void Func_801b664(unsigned char *p);
extern void Func_801b810(unsigned char *p);
extern int Func_801be80(unsigned char *p);

int Func_801b398(int a)
{
    unsigned char *p;
    volatile unsigned int *k;

    p = iwram_3001e98;
    Func_801b9ec(p, 0);
    k = &gKeyPress;
top:
    WaitFrames(1);
    if (*(unsigned short *)(p + (0xe8 << 2)) != 0)
        goto top;
    if (a != 0x3e7) {
        if (gKeyRepeat & 0x10) {
            Func_801b664(p);
        } else if (gKeyRepeat & 0x20) {
            Func_801b810(p);
        } else if (*k & 1) {
            return Func_801be80(p);
        }
    }
    if (a == 0)
        goto top;
    if ((*k & 2) == 0)
        goto top;
    return -1;
}
