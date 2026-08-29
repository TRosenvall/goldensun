extern void _Func_8019000(int win, int tile, int col, int row, int pal);

void Func_80a21b0(int win, int total, int perPage, int page, int col)
{
    int pages;
    int tile;
    int i;

    tile = 0x31;
    pages = total / perPage;
    if (total % perPage != 0) {
        pages++;
    }
    col -= pages;
    if (pages > 1) {
        _Func_8019000(win, 0xf128, col - 1, -1, 0);
        for (i = 0; i < pages; i++) {
            if (i == page) {
                _Func_8019000(win, tile, col, -1, 2);
            } else {
                _Func_8019000(win, tile, col, -1, 3);
            }
            tile++;
            col++;
        }
        _Func_8019000(win, 0xf129, col, -1, 0);
    }
}
