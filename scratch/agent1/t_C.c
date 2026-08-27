struct Spr {
    unsigned char pad00[0xf];
    unsigned char f0f;
};

extern int _GetUnit(int id);
extern void _Func_8016478(int win);
extern int _FindEmptyInventorySlot(int id);
extern void _DrawSmallText(int msg, int win, int x, int y);
extern struct Spr *_Func_801eb90(int item, int kind, int win, int x, int y);

void Func_80b1dec(int win, int id)
{
    unsigned short *w;
    unsigned int rec;
    struct Spr *s;
    int i;
    int x;
    int y;
    int item;

    rec = (unsigned int)_GetUnit(id);
    x = 8;
    y = 0;
    if (win != 0) {
        _Func_8016478(win);
        if (_FindEmptyInventorySlot(id) == 0) {
            _DrawSmallText(0xc91, win, 8, 0x14);
        } else {
            w = (unsigned short *)0xd8;
            for (i = 0; i <= 0xe; i++) {
                item = *(unsigned short *)((char *)w + rec);
                if (item != 0) {
                    s = _Func_801eb90(item, 0x1b, win, x, y);
                    s->f0f = 0xfc;
                }
                x += 0x10;
                if (i == 4) {
                    x = 8;
                    y += 0x10;
                }
                if (i == 9) {
                    x = 8;
                    y += 0x10;
                }
                w++;
            }
        }
    }
}
