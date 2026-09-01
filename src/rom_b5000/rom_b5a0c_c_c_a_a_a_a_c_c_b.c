extern char *iwram_3001e74;
extern int Func_80b6a60(unsigned short *buf);

int Func_80b6378(void)
{
    unsigned short buf[8];
    char *p;
    int i;
    int n;
    int k;

    p = iwram_3001e74;
    n = Func_80b6a60(buf);
    for (i = 0; i < n; i++) {
        k = buf[i] + 0x48;
        p[k] = i - 0x80;
    }
}
