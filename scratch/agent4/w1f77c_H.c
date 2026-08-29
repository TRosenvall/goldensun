extern unsigned int gKeyHeld;
extern unsigned char *iwram_3001f1c;
extern short ewram_2002010;
extern short ewram_200200c;
extern int Func_80056cc(void);
extern int Func_8005c68(void);
extern void Func_8005cf8(void);

int Func_801f77c(void)
{
    int n;
    int r;
    int t;
    signed char *p;
    int i;

    t = Func_80056cc();
    n = 0;
    r = -9;
    if (t == 0) {
        r = Func_8005c68();
        p = (signed char *)iwram_3001f1c;
        ewram_2002010 = 0;
        ewram_200200c = 0;
        p += 0x1070;
        i = 2;
        do {
            if (p[1] != 0) {
                ewram_2002010 = 1;
                n++;
            }
            if (p[2] != 0) {
                ewram_200200c = 1;
            }
            i--;
            p += 0x40;
        } while (i >= 0);
        if ((gKeyHeld & 0x120) != 0x120)
            ewram_2002010 = 0;
    }
    Func_8005cf8();
    if (r != 0 && n == r)
        return r + 100;
    return r;
}
