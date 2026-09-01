extern unsigned char *iwram_3001e64;
extern int Func_800eba0(void *p, int x, int b, int y);

unsigned char *Func_800d98c(unsigned char *a, int b)
{
    unsigned char *p;
    unsigned char *q;
    int i;

    p = iwram_3001e64;
    q = p;
    i = 0;
    q += 0x59;
    do {
        if (*(int *)p != 0 && (*q & 1) != 0 && p != a) {
            if (Func_800eba0(p + 8, *(unsigned short *)(p + 0x20) - 2, b,
                             *(unsigned short *)(a + 0x20) - 2) >= 0)
                return p;
        }
        i++;
        q += 0x70;
        p += 0x70;
    } while (i <= 0x3f);
    return 0;
}
