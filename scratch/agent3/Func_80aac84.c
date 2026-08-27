void Func_80aac84(int delta)
{
    int pass;
    int idx;
    int i;
    int off;
    int c;
    int r;
    int g;
    int b;

    pass = 0;
    idx = 0xf;
    do {
        idx <<= 4;
        i = 0;
        do {
            off = (idx + i) << 1;
            c = *(unsigned short *)(off + (0xa0 << 19));
            r = (c >> 10) & 0x1f;
            g = (c >> 5) & 0x1f;
            b = 0x1f & c;
            r += delta;
            g += delta;
            b += delta;
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
            *(unsigned short *)(off + 0x4ffffe0) = (r << 10) | (g << 5) | b;
            i++;
        } while (i <= 0xf);
        if (pass != 0) {
            delta = -12;
            idx = 7;
        } else {
            idx = 5;
        }
        pass++;
    } while (pass <= 2);
}
