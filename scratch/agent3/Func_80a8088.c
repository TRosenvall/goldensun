extern unsigned char *iwram_3001f2c;
extern int Func_80a10d0(void *slot, int a, int b, int c, int d, int e);
extern unsigned char *_Func_801ec6c(int page, int a, int b, void *win, int e, int f);
extern void Func_80a33d4(unsigned char *st, void *win);
extern void Func_80a9cf8(void *win);
extern void Func_80a8604(void *win, int page, int flag);

void Func_80a8088(int page)
{
    unsigned char *st;
    void *win;
    void **slot;
    unsigned char *node;
    int fresh;

    st = iwram_3001f2c;
    win = *(void **)(st + 0x24);
    fresh = 0;
    if (win == 0) {
        slot = (void **)(st + 0x24);
        fresh = Func_80a10d0(slot, 0, 5, 0x1e, 0xf, 2);
        win = *slot;
    }
    if (fresh != 0) {
        node = _Func_801ec6c(page, 0, 0, win, 0, 0);
        *(unsigned char **)(st + (0xbe << 1)) = node;
        node[0xf] = 0xf0;
        if (*(unsigned short *)(st + 0x220) == 3)
            Func_80a33d4(st, win);
        Func_80a9cf8(win);
        Func_80a8604(win, page, 0x100);
    } else {
        Func_80a8604(win, page, 0);
    }
}
