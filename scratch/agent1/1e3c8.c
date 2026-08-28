extern unsigned char *iwram_3001e8c;
void Func_801e3c8(int flag)
{
    unsigned char *base = iwram_3001e8c;
    unsigned char *p;
    int i, z;
    if (flag) {
        base[0xea2] = 1;
        z = 0;
        p = base + 0xe20;
        i = 0x80;
        do { i++; *p++ = z; } while (i <= 0xff);
    } else {
        base[0xea2] = 0;
        z = 0;
        p = base + 0xe20;
        i = 0x7f;
        do { i--; *p++ = z; } while (i >= 0);
    }
}
