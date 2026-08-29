#include "gba/types.h"

int Func_80f4100(u16 *src, u16 *dst, int scale, int count)
{
    u32 r, g, b, c;
    if (count > 0) {
        do {
            c = *src;
            r = ((c & 0x1f) * scale) >> 16;
            g = ((c & (0xf8 << 2)) * scale) >> 16;
            b = ((c & (0xf8 << 7)) * scale) >> 16;
            *dst = (r & 0x1f) | (g & (0xf8 << 2)) | (b & (0xf8 << 7));
            src++; dst++; count--;
        } while (count != 0);
    }
    return 0;
}
