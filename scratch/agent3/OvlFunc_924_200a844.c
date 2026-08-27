extern void __WaitFrames(int n);

void OvlFunc_924_200a844(void)
{
    volatile unsigned short *p;
    unsigned int n;
    unsigned int i;
    int r, g, b;

    do {
        p = (volatile unsigned short *)0x5000050;
        n = 0;
        i = 0;
        do {
            r = *p & 0x1f;
            g = (*p >> 5) & 0x1f;
            b = (*p >> 10) & 0x1f;
            if (r == 0x1f && g == 0x1f && b == 0x1f) {
                n++;
            } else {
                if (r <= 0x1e)
                    r++;
                if (g <= 0x1e)
                    g++;
                if (b <= 0x1e)
                    b++;
                *p = (b << 10) | (g << 5) | r;
            }
            i++;
            p++;
        } while (i <= 7);
        __WaitFrames(2);
    } while (n <= 7);
}
