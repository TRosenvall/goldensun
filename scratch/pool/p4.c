#include "gba/types.h"

int Func_80f4100(u16 *src, u16 *dst, int scale, int count)
{
    u32 r, g, b, c;
    if (count > 0) {
        do {
            c = *src++;
            r = (c & 0x1f) * scale;
            g = (c & (0xf8 << 2)) * scale;
            b = (c & (0xf8 << 7)) * scale;
            r = (r >> 16) & 0x1f;
            g = (g >> 16) & (0xf8 << 2);
            r |= g;
            b = (b >> 16) & (0xf8 << 7);
            r |= b;
            *dst++ = r;
        } while (--count != 0);
    }
    return 0;
}
