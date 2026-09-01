extern unsigned char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);
extern void Func_80a3d24(void *p);

void Func_80a68a8(unsigned short *src)
{
    unsigned char *p;
    unsigned char **q;
    unsigned char *node;
    unsigned short *s;
    int i;
    int v;

    p = iwram_3001f2c;
    q = (unsigned char **)(p + 0x48);
    s = src;
    i = 0x1f;
    do {
        v = *s++;
        if (v != 0) {
            node = *q;
            _Func_801bcd4(4, v, node[0xe], 0);
        }
        i--;
        q++;
    } while (i >= 0);
    Func_80a3d24(src);
}
