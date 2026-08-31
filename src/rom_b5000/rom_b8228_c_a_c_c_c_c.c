extern int iwram_3001e80[];
extern volatile int gKeyHeld;
extern void Func_80c0a24(int a, int b, int c, int d, int e);

void Func_80b9acc(void)
{
    char *p;
    char *q;

    p = (char *)iwram_3001e80[0];
    q = (char *)iwram_3001e80[0x20];
    if (gKeyHeld & (0x80 << 2))
        *(unsigned short *)(p + 0x36) += 0x80 << 2;
    if (gKeyHeld & (0x80 << 1))
        *(unsigned short *)(p + 0x36) -= 0x200;
    if (*(int *)(q + 0x14) == 0)
        Func_80c0a24(0xf0 << 15, 0xf0 << 15, 0, 0, 0x80 << 9);
}
