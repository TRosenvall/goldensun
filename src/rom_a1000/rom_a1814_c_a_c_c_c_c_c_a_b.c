extern unsigned char *iwram_3001f2c;
extern void Func_80a17c4(void *node);

void Func_80a3d24(unsigned short *src)
{
    unsigned char *state;
    unsigned short *s;
    int off;
    int i;
    int k;

    state = iwram_3001f2c;
    off = 0x48;
    s = src;
    k = 0xd;
    i = 0x1f;
    do {
        if (*s++ == 0) {
            Func_80a17c4(*(void **)(off + (int)state));
            (*(unsigned char **)(off + (int)state))[5] = k;
        }
        i--;
        off += 4;
    } while (i >= 0);
}
