extern unsigned int iwram_3001e40;
extern unsigned char L9e6b8[] __asm__(".L9e6b8");

void Func_808e0b0(unsigned char *e, int visible)
{
    unsigned char *o;
    unsigned char **list;
    unsigned char *q;
    int v;
    int n;
    unsigned char *cp;

    if ((*(unsigned char *)(e + 0x54) & 0xf) != 1)
        return;
    o = *(unsigned char **)(e + 0x50);
    v = visible - 1;
    if (visible == 0)
        v = L9e6b8[(iwram_3001e40 >> 1) & 7];
    cp = o + 0x27;
    if (*cp != 0) {
        list = (unsigned char **)(o + 0x28);
        n = *cp;
        do {
            q = *list++;
            if (q != 0 && *(int *)(q + 0x10) != 0 && q[5] != 0xf)
                q[5] = v;
            n--;
        } while (n != 0);
    }
    o[0x25] = 1;
}
