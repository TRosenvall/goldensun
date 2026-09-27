/* Func_809b8f4  --  0x0809b8f4, was asm/rom_8a000/rom_9b698_a_a_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * AN int CARRIER KEEPS A ZERO-EXTENSION combine WOULD DROP: where the ROM
 * zero-extends and then re-sign-extends a short (lsl/lsr, then lsl/asr), write
 * `int u = (unsigned short)a; ...; a = (short)u;`. An `unsigned short u` lets
 * combine drop both extensions (46 differing -> exact).
 */
#include "math.h"

extern int Func_8000948(int v);
extern int FastIntSqrtFP1616_RAM(int v);
extern int atan2(int y, int x);
extern void Func_809ba5c(unsigned char *e, int x, int y);

void Func_809b8f4(unsigned char *e)
{
    int dx;
    int dz;
    int d;
    int a;
    int sp;
    int diff;
    int lim;
    int ad;
    int cur;
    int u;
    int (*fp)(int);

    if (*(int *)(e + 0xc) == (int)0x80000000)
        return;
    dx = *(int *)(e + 0xc) - *(int *)(e + 4);
    dz = *(int *)(e + 0x10) - *(int *)(e + 8);
    if (*(signed char *)(e + 0x41) != 0) {
        int x = dx / 0x10000;
        int z = dz / 0x10000;
        int sq = x * x + z * z;
        fp = Func_8000948;
        d = fp(sq) << 16;
        if (d < 0x80 << 16)
            d = FastIntSqrtFP1616_RAM(fx32_multiply(dx, dx) + fx32_multiply(dz, dz));
        if (d <= 0x80 << 12) {
            Func_809ba5c(e, *(int *)(e + 0xc), *(int *)(e + 0x10));
            return;
        }
    }
    a = (short)atan2(dz, dx);
    if (*(signed char *)(e + 0x42) != 0) {
        cur = *(unsigned short *)(e + 0x30);
        diff = (short)(a - cur);
        ad = diff < 0 ? -diff : diff;
        lim = *(short *)(e + 0x32);
        if (ad >= lim) {
            if (diff < 0) {
                if (-diff > lim)
                    diff = (short)-*(unsigned short *)(e + 0x32);
            } else if (diff > lim) {
                diff = lim;
            }
            a = (short)(diff + cur);
        }
    }
    u = (unsigned short)a;
    *(unsigned short *)(e + 0x30) = u;
    sp = *(int *)(e + 0x1c) + *(int *)(e + 0x24);
    if (sp > *(int *)(e + 0x20))
        sp = *(int *)(e + 0x20);
    *(int *)(e + 0x1c) = sp;
    a = (short)u;
    *(int *)(e + 4) += fx32_multiply(cos(a), sp);
    *(int *)(e + 8) += fx32_multiply(sin(a), sp);
}
