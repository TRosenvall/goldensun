extern unsigned int iwram_3001e40;
extern unsigned char L9e6b8[] __asm__(".L9e6b8");

struct O {
    unsigned char pad00[0x1d];
    unsigned char flags;
    unsigned char pad1e[7];
    unsigned char f25;
    unsigned char pad26;
    unsigned char count;
    unsigned char *list[1];
};

void Func_808e0b0(unsigned char *e, int visible)
{
    struct O *o;
    unsigned char **list;
    unsigned char *q;
    int v;
    int n;

    if ((*(unsigned char *)(e + 0x54) & 0xf) != 1)
        return;
    o = *(struct O **)(e + 0x50);
    v = visible - 1;
    if (visible == 0)
        v = L9e6b8[(iwram_3001e40 >> 1) & 7];
    if (o->count != 0) {
        list = o->list;
        n = o->count;
        do {
            q = *list++;
            if (q != 0 && *(int *)(q + 0x10) != 0 && q[5] != 0xf)
                q[5] = v;
            n--;
        } while (n != 0);
    }
    o->f25 = 1;
}
