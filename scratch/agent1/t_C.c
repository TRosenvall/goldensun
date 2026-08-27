extern unsigned int iwram_3001e8c;

struct Win {
    unsigned char pad00[8];
    unsigned short w;
    unsigned short h;
    unsigned short x;
    unsigned short y;
};

void Func_8019000(struct Win *win, int entry, unsigned int col, unsigned int row,
                  unsigned int mode)
{
    unsigned char *map;
    unsigned int pal;
    unsigned int idx;
    unsigned short *q;

    map = *(unsigned char **)&iwram_3001e8c;
    q = (unsigned short *)map;
    row++;
    col++;
    if (row > (unsigned int)(win->h - 1))
        return;
    if (col > (unsigned int)(win->w - 1))
        return;
    switch (mode) {
    case 2:
        pal = 0xe0 << 8;
        break;
    case 3:
        pal = 0xf0 << 8;
        break;
    case 4:
        pal = 0x80 << 5;
        break;
    default:
        pal = 0;
        break;
    }
    if (mode == 1)
        return;
    if (mode >= 1) {
        if (mode <= 4) {
            idx = ((win->y + row) << 5) + (win->x + col);
            if (idx >= (0xa0 << 2))
                return;
            q[idx] = pal | entry;
            return;
        }
    }
    idx = ((win->y + row) << 5) + (win->x + col);
    if (idx >= (0xa0 << 2))
        return;
    q[idx] = entry;
}
