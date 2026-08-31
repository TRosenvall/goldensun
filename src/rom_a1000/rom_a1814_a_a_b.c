extern int iwram_3001f2c;
extern int _Func_80796c4(void *out);
extern void _DeleteSprite(void *s);
extern void StopTask(void *fn);
extern void Func_80a19a0(void);

void Func_80a195c(void)
{
    char *base;
    void **p;
    void *s;
    int c;
    int n;
    int buf[7];

    base = (char *)iwram_3001f2c;
    c = (unsigned short)_Func_80796c4(buf);
    if (c != 0) {
        p = (void **)(base + (0x8a << 1));
        n = c;
        do {
            s = *p++;
            if (s != 0)
                _DeleteSprite(s);
            n--;
        } while (n != 0);
    }
    StopTask(Func_80a19a0);
}
