extern unsigned char *iwram_3001f1c;
extern int Func_8005b24(void);
extern int Random(void);

int Func_8005810(void)
{
    int v[16];
    unsigned char *p;
    int cnt;
    unsigned int r;

    p = iwram_3001f1c;
    cnt = 0;
    for (r = 0; r <= 0xf; r++) {
        if (*p++ == 0) {
            v[cnt] = r;
            cnt++;
        }
    }
    r = 0x10;
    if (cnt != 0) {
        if (cnt == 1) {
            r = v[0];
            if (Func_8005b24() == 0x10)
                r = 0x10;
        } else {
            r = (unsigned int)Random() % cnt;
            r = v[r];
        }
    }
    return r;
}
