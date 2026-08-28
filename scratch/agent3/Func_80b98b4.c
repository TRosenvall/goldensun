typedef unsigned short u16;

void Func_80b98b4(int add)
{
    int i, j, row, base;
    int idx, r, g, b;
    unsigned int c;

    row = 15;
    i = 0;
    do {
        base = row << 4;
        j = 0;
        while (1) {
            idx = base + j;
            c = ((u16 *)0x05000000)[idx];
            b = (c >> 10) & 31;
            g = (c >> 5) & 31;
            r = c & 31;
            b += add;
            g += add;
            r += add;
            if (b > 31) b = 31;
            if (g > 31) g = 31;
            if (r > 31) r = 31;
            if (b < 0) b = 0;
            if (g < 0) g = 0;
            if (r < 0) r = 0;
            ((u16 *)0x05000000)[idx - 16] = (b << 10) | (g << 5) | r;
            j++;
            if (j > 15) break;
        }
        row = 5;
        i++;
    } while (i <= 1);
}
