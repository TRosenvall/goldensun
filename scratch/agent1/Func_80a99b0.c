int Func_80a99b0(int *pcol, int *prow, int dir)
{
    int col;
    int row;

    col = *pcol;
    row = *prow;
    switch (dir) {
    case 0x40:
        row--;
        if (row < 0)
            row = 5;
        if (row > 3)
            break;
        if (row == 3) {
            if (col > 4)
                col = 1;
            else
                col = 0;
        } else {
            if (col > 1)
                col = 1;
        }
        if (row == 3 && col == 1)
            row = 2;
        break;
    case 0x80:
        row++;
        if (row > 5)
            row = 0;
        if (row == 3 && col == 1)
            row = 4;
        if (row == 4)
            col = 0;
        break;
    case 0x20:
        col--;
        if (row == 3) {
            col++;
        } else if (row > 3) {
            if (col < 0)
                col = 7;
        } else {
            if (col < 0)
                col = 1;
        }
        break;
    case 0x10:
        col++;
        if (row == 3) {
            col--;
        } else if (row > 3) {
            if (col > 7)
                col = 0;
        } else {
            if (col > 1)
                col = 0;
        }
        break;
    }
    *pcol = col;
    *prow = row;
    return row * 9 + col;
}
