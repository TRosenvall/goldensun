extern char *iwram_3001f30;
extern char *iwram_3001e64;

extern void Func_809bb34(void *a);
extern void WaitFrames(int n);
extern int StopTask(void *t);
extern void Func_8003f3c(int a);
extern void Func_809202c(void);
extern void gfree(int n);
extern void Func_8096d84(void);
extern void Func_8096d2c(void);
extern void Func_8096f8c(void);

void Func_8097194(void)
{
    char *p;
    char *st;
    char *tg;
    char *a;
    char *b;
    char *t;
    void *fn;
    int i;
    int n;
    int found;
    int c;

    p = iwram_3001f30;
    st = *(char **)((char *)&iwram_3001f30 - 0x74);
    tg = *(char **)((char *)&iwram_3001f30 - 0xc0);
    a = p + 0x9d;
    b = p + 0x58;
    i = 0x17;
    do {
        c = *(signed char *)a;
        a += 0x48;
        if (c != 0)
            Func_809bb34(b);
        i--;
        b += 0x48;
    } while (i >= 0);
    if (*(signed char *)(st + 0xcc6) == 0) {
        n = 0;
        do {
            t = iwram_3001e64;
            found = 0;
            i = 0;
            while (i <= 0x3f) {
                fn = *(void **)(t + 0x6c);
                if (fn == (void *)Func_8096d84) {
                    found = 1;
                    break;
                }
                i++;
                t += 0x70;
                if (fn == (void *)Func_8096d2c) {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
                break;
            n++;
            WaitFrames(1);
        } while (n <= 0x1d);
        st[0xcc7] = 0;
        StopTask((void *)Func_8096f8c);
        Func_8003f3c(*(short *)(p + 0x46));
        *(int *)(tg + 4) = *(int *)(p + 0x4c);
        *(int *)(tg + 8) = *(int *)(p + 0x50);
        *(int *)(tg + 0xc) = *(int *)(p + 0x54);
        if (*(short *)(p + 0x1e) != 8)
            *(short *)(st + (0xcc << 4)) = 1;
        Func_809202c();
        gfree(0x38);
    }
}
