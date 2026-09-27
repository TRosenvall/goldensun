/* Func_801868c  --  0x0801868c, split out of asm/rom_15000/rom_17e88_a.s;
 * Func_8017e88 and BufferString stay in _a.s. Matched from scratch.
 *
 * - `if (t >= 0) x = t; else x = 0;` keeps the ROM's separate `sub r3,.. / mov
 *   r6,r3` copy. cse_insn's copy swap (cse.c:5972) folds `t = ...; x = t;` into
 *   `x = ...` whenever the setter is the previous non-note insn and x outlives
 *   t -- and a do{}while(0) between them does NOT stop it, since notes are
 *   skipped.
 * - A separate `t -= lim; x -= t;` stops fold turning x - (t - k) into
 *   (x - t) + k; `(int)(x + *w) > lim` gives the signed first compare.
 */
extern unsigned char *iwram_3001e8c;
extern void Func_8018850(int a, unsigned int *w, unsigned int *h, int d);
extern void Func_8018a50(int a, unsigned int *w, unsigned int *h, int d);

void Func_801868c(int a, int *px, int *py, unsigned int *w, unsigned int *h, int d, int flags)
{
    unsigned char *base;
    int lim;
    int x, y;
    int t;

    base = iwram_3001e8c;
    x = *px;
    y = *py;
    lim = 0x1e;
    if ((flags & 2) == 0) {
        if (flags & 1)
            Func_8018a50(a, w, h, d);
        else
            Func_8018850(a, w, h, d);
    }
    if (*w != 0 || *h != 0) {
        if ((flags & 2) == 0) {
            *w = (*w + 0x13) >> 3;
            *h = (*h + 0xf) >> 3;
            if (base[0xea4] != 0) {
                *w += 2;
                lim = 0x1d;
            }
        }
        t = x + *w;
        if (t > lim) {
            t -= lim;
            t = x - t;
            if (t >= 0)
                x = t;
            else
                x = 0;
        }
        t = y + *h;
        if (t > 0x14) {
            t -= 0x14;
            t = y - t;
            if (t >= 0)
                y = t;
            else
                y = 0;
        }
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        if (x > lim - *w)
            x = lim - *w;
        if (y > 0x14 - *h)
            y = 0x14 - *h;
        *px = x;
        *py = y;
    }
}
