extern unsigned int gState;
extern int __GetPartySize(void);

void OvlFunc_971_200853c(short *out)
{
    unsigned char *p;
    unsigned int g;
    unsigned int off;
    int n;
    int i;
    int v;

    n = __GetPartySize();
    if (n > 3)
        n = 3;
    if (n > 0) {
        g = (unsigned int)&gState;
        off = 0xfc;
        off <<= 1;
        g += off;
        p = (unsigned char *)g;
        i = n;
        while (i != 0) {
            v = *p;
            p++;
            if (out != 0) {
                *out = v;
                out++;
            }
            i--;
        }
    }
    if (out != 0)
        *out = 0xff;
}
