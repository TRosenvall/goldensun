#include "gba/types.h"

int Func_80f4100(u16 *src, u16 *dst, int scale, int count)
{
    u32 r, g, b, c;
    int i;
    for (i = 0; i < count; i++) {
        c = src[i];
        r = (c & 0x1f) * scale;
        g = (c & (0xf8 << 2)) * scale;
        b = (c & (0xf8 << 7)) * scale;
        dst[i] = ((r >> 16) & 0x1f) | ((g >> 16) & (0xf8 << 2)) | ((b >> 16) & (0xf8 << 7));
    }
    return 0;
}
