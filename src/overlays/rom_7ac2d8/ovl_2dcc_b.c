extern void __WaitFrames(int n);

void OvlFunc_924_200ae08(void)
{
    volatile unsigned short *p;
    unsigned int n;
    unsigned int i;
    int r, g, b;

    do {
        p = (volatile unsigned short *)0x50000c2;
        n = 0;
        i = 0;
        do {
            r = *p & 0x1f;
            g = (*p >> 5) & 0x1f;
            b = (*p >> 10) & 0x1f;
            if (r > 0)
                r--;
            if (g > 0)
                g--;
            if (b > 0)
                b--;
            *p = (b << 10) | (g << 5) | r;
            if (*p == 0)
                n++;
            i++;
            p++;
        } while (i <= 6);
        __WaitFrames(5);
    } while (n != 7);
}
