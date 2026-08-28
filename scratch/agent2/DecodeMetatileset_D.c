typedef unsigned char u8;
typedef unsigned short u16;

extern u8 ewram_2010001[];
extern u8 ewram_2010002[];
extern u8 ewram_2020000[];

void DecodeMetatileset(int count)
{
    int i;
    u16 *src;
    u16 *dst;
    u8 *hi;
    u8 *lo;
    int v;
    int prev;
    int prev2;
    int n;

    n = (count - 1) / 2;
    src = (u16 *)ewram_2010002;
    dst = (u16 *)ewram_2020000;
    if (count & 1) {
        switch (ewram_2010001[0]) {
        case 0:
            for (i = 0; i < n; i++) {
                *dst = *src;
                src++;
                dst++;
            }
            break;
        case 1:
            hi = ewram_2010002;
            prev = 0;
            lo = hi + n;
            for (i = 0; i < n; i++) {
                v = *hi;
                v <<= 8;
                v |= *lo;
                v ^= prev;
                *dst = v;
                lo++;
                hi++;
                dst++;
                prev = v;
            }
            break;
        case 2:
            prev2 = 0;
            for (i = 0; i < n; i++) {
                v = *src;
                v ^= prev2;
                *dst = v;
                src++;
                dst++;
                prev2 = v;
            }
            break;
        }
    }
}
