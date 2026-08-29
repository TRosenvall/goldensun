extern char *iwram_3001e8c;
extern void Func_801671c(void);

void Func_8016758(void)
{
    char *p;
    char *found;
    char *rec;
    int i;

    p = iwram_3001e8c + (0xc4 << 3);
    found = 0;
    i = 0;
    while (i != 3) {
        rec = *(char **)p;
        if (rec == 0 || *(unsigned short *)(rec + 0x14) != 0) {
            found = p;
            break;
        }
        p += 0x28;
        i++;
    }
    if (found == 0)
        return;
    if (*(char **)found != 0) {
        Func_801671c();
        *(unsigned short *)(found + 6) = 0;
    }
    *(unsigned short *)(found + 4) = 0;
    *(unsigned short *)(found + 0x14) = 0;
    *(unsigned short *)(found + 0x18) = 0;
    *(unsigned short *)(found + 0x16) = 0xf;
    *(unsigned short *)(found + 0x1a) = 0xa;
}
