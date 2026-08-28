extern unsigned char ewram_2010001[];
extern unsigned char ewram_2010002[];
extern unsigned char ewram_2020000[];

void DecodeMetatileset(int n)
{
    unsigned short *dst;
    unsigned short *src;
    unsigned char *hi, *lo;
    int count, i, a, b;
    int prev1, v1;
    int prev2, v2;

    count = (n - 1) / 2;
    src = (unsigned short *)ewram_2010002;
    dst = (unsigned short *)ewram_2020000;
    if ((n & 1) != 0) {
        switch (ewram_2010001[0]) {
        case 0:
            for (i = 0; i < count; i++) {
                *dst = *src;
                src++;
                dst++;
            }
            break;
        case 1:
            hi = (unsigned char *)0x2010002;
            prev1 = 0;
            lo = count + hi;
            for (i = 0; i < count; i++) {
                a = *hi;
                b = *lo;
                a <<= 8;
                v1 = (a | b) ^ prev1;
                *dst = v1;
                lo++;
                hi++;
                dst++;
                prev1 = v1;
            }
            break;
        case 2:
            prev2 = 0;
            for (i = 0; i < count; i++) {
                v2 = *src ^ prev2;
                *dst = v2;
                src++;
                dst++;
                prev2 = v2;
            }
            break;
        }
    }
}
