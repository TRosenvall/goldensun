/* Func_80b6a60 (0x080b6a60) -- NON-MATCHING.
 * Blocker class: allocation ORDER -- the same rotation as
 * rom_15000/801a66c.c and rom_15000/8020150.c.
 *
 * 61 lines against 61, 14 differing. The r8 that holds the constant 2 across
 * the loop's `_GetUnit` call comes out unprompted. What differs is which of the
 * two remaining long-lived values gets the higher register:
 *
 *              the party count   the gState walker
 *     rom      r7                r2
 *     ours     r2                r7
 *
 * The count is created first (it is the return of `_GetPartySize`) and the ROM
 * gives it the HIGHER register; gcc gives the earlier-created value the lower
 * one. Third specimen of the same descending-versus-ascending pattern.
 *
 * MEASURED (rom 61 lines):
 *   `g = gState + (0xfc << 1);`                    58, 39 -- folds to a pooled
 *                          `gState+504` where the ROM has `ldr =gState` plus a
 *                          separate `mov`/`lsl`/`add`
 *   `g = gState; g += 0xfc << 1;` as two statements 61, 14  <- best
 *   `i--;` moved before the `u[0x12a] = m;` store
 *     to match the ROM's order                      61, 14 (inert)
 *
 * WHAT IS RIGHT: the r8 constant; the two-statement gState base; the `lim = 4;
 * if (...) lim = 3;` clamp; the optional output pointer tested twice; and the
 * pooled 0xff for the terminator halfword, which gcc pools from a plain literal.
 *
 * NEXT: nothing source-level.
 */
extern unsigned char *iwram_3001e74;
extern unsigned char gState[];
extern int _GetPartySize(void);
extern unsigned char *_GetUnit(int id);

int Func_80b6a60(unsigned short *out)
{
    unsigned char *b;
    unsigned char *g;
    unsigned char *u;
    int lim;
    int n;
    int i;
    int m;
    int id;

    b = iwram_3001e74;
    lim = 4;
    if (*(b + 0x44) != 0)
        lim = 3;
    n = _GetPartySize();
    if (n > lim)
        n = lim;
    if (n > 0) {
        g = gState;
        g += 0xfc << 1;
        m = 2;
        i = n;
        do {
            id = *g;
            g++;
            if (out != 0) {
                *out = id;
                out++;
            }
            u = _GetUnit(id);
            i--;
            u[0x95 << 1] = m;
        } while (i != 0);
    }
    if (out != 0)
        *out = 0xff;
    return n;
}
