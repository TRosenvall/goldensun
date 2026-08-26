#include "gba/types.h"

int Func_80f4100(u16 *src, u16 *dst, int scale, int count)
{
    u32 c;
    if (count > 0) {
        do {
            c = *src;
            *dst = ((((c & 0x1f) * scale) >> 16) & 0x1f)
                 | ((((c & (0xf8 << 2)) * scale) >> 16) & (0xf8 << 2))
                 | ((((c & (0xf8 << 7)) * scale) >> 16) & (0xf8 << 7));
            src++; dst++; count--;
        } while (count != 0);
    }
    return 0;
}
