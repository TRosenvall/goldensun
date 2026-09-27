/* Func_8079b24  --  0x08079b24, was asm/rom_77000/rom_79460_c_c_c_c_a_a_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * - Naming the global table directly inside the loop makes loop.c hoist the
 *   symbol into a fresh pseudo: the ROM's `mov r12, r5`.
 * - Offsets through a `char *p` local give base-first `[base, k]`; through the
 *   symbol they give `[k, base]`. Named offset temps (`k = o - 4; T(k)`) stop
 *   fold reassociating `(p + o) - 4`.
 * - DECLARATION ORDER WAS NOT INERT HERE although both locals are
 *   register-resident: swapping hi/lo took 17 differing to 11 (batch 285 found
 *   it inert across 180 compiles on three other functions). The last k-register
 *   issue closed by splitting `k` into two variables at the right statements.
 */
struct Pt { short x; short y; };
extern struct Pt L89258[] __asm__(".L89258");
#define T(o) (*(short *)(p + (o)))

int Func_8079b24(int x, int mode)
{
    int lo;
    int hi;
    int n;
    int i;
    int r;
    int o;
    int k;
    int k2;
    char *p;
    int x0, x1, y0, y1;

    hi = L89258[0].x;
    lo = L89258[4].x;
    n = 5;
    if (x > hi)
        x = hi;
    else if (x < lo)
        x = lo;
    for (i = 0; i < n; i++)
        if (x > L89258[i].x)
            break;
    p = (char *)L89258;
    o = i * 4;
    if (i == n) {
        k = o - 2;
        r = T(k);
    } else {
        k = o - 4;
        x0 = T(k);
        x1 = T(o);
        k = o - 2;
        y0 = T(k);
        k2 = o + 2;
        y1 = T(k2);
        x0 -= x1;
        y0 -= y1;
        x1 = x - x1;
        x1 *= y0;
        r = x1 / x0 + y1;
    }
    switch (mode) {
    case 0:
        break;
    case 1:
        r /= 2;
        break;
    }
    return r + 0x100;
}
