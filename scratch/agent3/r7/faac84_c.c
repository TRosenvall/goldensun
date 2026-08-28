void Func_80aac84(int delta)
{
    int i;
    int j;
    int n;
    int idx;
    int r, g, b;
    unsigned int c;
    int off;

    n = 0xf;
    i = 0;
    do {
        idx = n << 4;
        for (j = 0; j <= 0xf; j++) {
            off = (idx + j) << 1;
            c = *(unsigned short *)(0x5000000 + off);
            r = ((c >> 10) & 0x1f) + delta;
            g = ((c >> 5) & 0x1f) + delta;
            b = (c & 0x1f) + delta;
            if (r > 0x1f)
                r = 0x1f;
            if (g > 0x1f)
                g = 0x1f;
            if (b > 0x1f)
                b = 0x1f;
            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
            *(unsigned short *)(0x4ffffe0 + off) = (r << 10) | (g << 5) | b;
        }
        n = 5;
        if (i != 0) {
            delta = -12;
            n = 7;
        }
        i++;
    } while (i <= 2);
}
