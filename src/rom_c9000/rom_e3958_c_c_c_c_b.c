/* Func_80e46f0  --  0x080e46f0, split out of asm/rom_c9000/rom_e3958_c_c_c_c.s (Anim_Attack,
 * BaseAnim_Attack and Anim_CriticalHit stay in _a.s, the .rodata in _c.s; four
 * data labels were exported for the split). Matched from scratch.
 *
 * FAKEMATCH -- UploadPalette pins r0/r2 as rom_cc5d8_a_a_b.c does, booked in
 * fakematch.txt.
 *
 * THE INLINE-PARAMETER LEVER: without it gcc computes &buf for the stack array
 * once, before the first call, and keeps it alive to the last -- a wasted
 * callee-saved register that shifts everything. The ROM passes `mov r0,sp` /
 * `mov r1,sp` at each call and makes a fresh copy only after the first call.
 * Routing BOTH calls' buf argument through a `static inline` parameter gives a
 * new (set reg (plus sfp -128)) after the call. Only one site inlined leaves 11.
 * do{}while(0), register pins on the argument, a pointer alias, *buf or
 * &buf[0] were all inert. The loop body is Func_80f61e8's verbatim.
 */
#include "gba/types.h"
typedef void (*CopyFn)(void *dst, void *src, int len);
extern void *GetFile(int id);
extern void Func_8001af8(void *dst, void *src, int len);

static inline void UploadPalette(void *src)
{
    register u32 a0 __asm__("r0");
    register int a2 __asm__("r2");
    CopyFn f;
    a0 = 0xa0;
    f = Func_8001af8;
    a2 = 0x80;
    a0 <<= 19;
    f((void *)a0, src, a2);
}
static inline void CopyBuf(void *dst, void *src, int n)
{
    CopyFn f = Func_8001af8;
    f(dst, src, n);
}

void Func_80e46f0(int id)
{
    u16 buf[0x40];
    u16 *pal;
    void *src;
    u32 c;
    u32 d;
    int r;
    int g;
    int b;
    int r2;
    int g2;
    int b2;
    int i;
    int m;

    pal = (u16 *)0x5000000;
    src = GetFile(id);
    CopyBuf(buf, src, 0x80);
    buf[0] = 0;
    i = 0;
    m = 0x1f;
    do {
        c = *pal;
        b = m & c;
        g = (u16)((c << 16) >> 21) & 0x1f;
        r = (u16)((c << 16) >> 26) & 0x1f;
        d = buf[i];
        b2 = m & d;
        g2 = (u16)((d << 16) >> 21) & 0x1f;
        r2 = (u16)((d << 16) >> 26) & 0x1f;
        if (b < b2)
            b++;
        else if (b > b2)
            b--;
        if (g < g2)
            g++;
        else if (g > g2)
            g--;
        if (r < r2)
            r++;
        else if (r > r2)
            r--;
        buf[i] = (r << 10) | (g << 5) | b;
        i++;
        pal++;
    } while (i != 0x40);
    UploadPalette(buf);
}
