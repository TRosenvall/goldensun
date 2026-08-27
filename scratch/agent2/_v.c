char *PrintNum(char *buf, int v, unsigned int flags)
{
    char *p;
    int i;
    int neg;
    int spc;
    int minus;

    neg = 0;
    if (v < 0) {
        if (flags == 0) {
            neg = 1;
        }
        v = -v;
    }
    buf[0] = 0x20;
    i = 0xc;
    do {
        buf[i] = (v % 10) + 0x30;
        v = v / 10;
        i--;
    } while (i != 0);

    spc = 0x20;
    minus = 0x2d;
    buf[0xd] = 0;
    i = 1;
    p = buf;
    goto test;
inc:
    p++;
    i++;
test:
    if (i == 0xd) goto done;
    if (p[1] == 0x30) {
        if (i == 0xc) goto inc;
        p[1] = spc;
        goto inc;
    }
    if (neg != 0) {
        *p = minus;
    }
done:
    if (flags == 0) {
        i = 0;
        if (buf[0] == 0x20) {
            p = buf;
            do {
                i++;
                if (i == 0xc) break;
                p++;
            } while (*p == 0x20);
        }
        return buf + i;
    }
    if (flags > 0xc) {
        flags = 0xc;
    }
    return buf - flags + 0xd;
}
