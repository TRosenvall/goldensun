extern unsigned int iwram_3001e40;
extern unsigned char L9e6b8[] __asm__(".L9e6b8");

void Func_808e0b0(unsigned char *e, int visible)
{
    unsigned char *ent;
    unsigned char *q;
    int t;
    int m;
    int v;
    int n;
    int i;
    unsigned char **p;
    unsigned char *o;

    t = e[0x54];
    m = 0xf;
    m &= t;
    if (m == 1) {
        ent = *(unsigned char **)(e + 0x50);
        v = visible - 1;
        if (visible == 0)
            v = L9e6b8[(iwram_3001e40 >> 1) & 7];
        n = ent[0x27];
        if (n != 0) {
            p = (unsigned char **)(ent + 0x28);
            i = n;
            do {
                o = *p++;
                if (o != 0 && *(int *)(o + 0x10) != 0 && o[5] != 0xf)
                    o[5] = v;
                i--;
            } while (i != 0);
        }
        ent[0x25] = 1;
    }
}
