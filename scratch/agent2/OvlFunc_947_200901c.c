extern unsigned char iwram_3001e70[];

struct Cell {
    unsigned char b0;
    unsigned char lo : 4;
    unsigned char mid : 2;
    unsigned char hi : 2;
    unsigned char b2;
    unsigned char b3;
};

void OvlFunc_947_200901c(unsigned int layer, int x, int z, struct Cell *s)
{
    unsigned char *base;
    struct Cell *p;
    unsigned int off;

    base = *(unsigned char **)iwram_3001e70;
    if (base == 0)
        return;
    off = layer * 0x30 + 0x130;
    p = *(struct Cell **)(base + off);
    p += x + (z << 7);
    p->mid = s->mid;
    p->hi = s->hi;
    p->b2 = s->b2;
    p->b3 = s->b3;
}
