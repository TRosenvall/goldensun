/* Cluster Func_801ef68..Func_801ef68 extracted from goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c.s.
 *
 * Total .text for this TU = 288 bytes (= 0x120). Never attempted before batch 277.
 * No pins, no flags.
 *
 * THIS FUNCTION IS THE WORKED EXAMPLE FOR gcc-2.96's global_alloc PRIORITY FORMULA, which
 * batch 277 confirmed exactly:
 *
 *     priority = floor_log2(REG_N_REFS) * REG_N_REFS / REG_LIVE_LENGTH
 *
 * Both inputs are printed verbatim by `-da` in `.17.lreg` ("Register N used R times across L
 * insns"), and REG_N_REFS is LOOP-WEIGHTED as `+= loop_depth + 1` -- a depth-2 reference is
 * worth 3, depth-1 worth 2, depth-0 worth 1, and a def counts as a ref. On this function the
 * formula predicted `.18.greg`'s `;; 19 regs to allocate:` line IN FULL, every allocno, in
 * all five variants checked. See docs/elevation.md.
 *
 * WHY THAT MATTERED HERE. At 32 differing the structure was already exact and the residue was
 * {p,i,y} in {r0,r5,r6} against the ROM's {r6,r5,r0}. ELEVEN spellings measured EXACTLY 32 --
 * a struct for `p`, `int`/`unsigned` on every local, `<< 5` against `* 32`, operand order,
 * casts on `h1`, a split tail pointer, declaration permutations. The arithmetic says why:
 * `i` = 4*20/112 = 0.714, `p` = 3*12/64 = 0.5625, `y` = 2*7/25 = 0.560, so **`p` and `y` were
 * tied to within 0.4%** and no local respelling could separate them.
 *
 * TWO LEVERS, each necessary and neither sufficient:
 *
 * 1. THE TAIL LOOP'S COUNTER MUST BE `y`, the outer loop's column variable, not a fresh
 *    local. A separate `k` scores 8/16 = 1.50 and steals r3, which the ROM leaves as reload
 *    scratch. 57 -> 41.
 *
 * 2. `i = n;` AS A MERGE-POINT COPY THROUGH AN EXTRA LOCAL -- `n = 1; if (flags & 2) { d = 5;
 *    n = 0; } i = n;`. This starts `i`'s live range at the `if`'s merge point and cut it from
 *    12 refs / 96 insns to 12 / 40, i.e. 0.375 -> 0.900, moving `i` from 11th to 4th in the
 *    allocation order. Together with lever 1: 41 -> 12.
 *
 *    `i = n;` IS A NEW LEVER AND THE CHEAPEST KNOWN WAY TO SHORTEN A LOOP INDEX'S LIVE RANGE.
 *    An `if/else` writing `i` in both arms is 80; a ternary is 40 but costs three
 *    instructions.
 *
 * AND THE COROLLARY, which is the trap: A "DEAD" INITIALISER CANNOT BUY PRIORITY. `y = 0;`
 * before the loop is deleted outright, and moving the extra reference AFTER the loop
 * ballooned `y`'s live length from 25 to 78 and made things worse. The extra reference has to
 * be INSIDE the loop or it is not a lever at all.
 *
 * The last 12 were entry-block scheduling. A 120-way permutation of the five leading
 * statements found TWO exact spellings -- (base, n, d, h1, w) and (n, base, d, h1, w) -- with
 * the rest of the field spread from 6 to 56.
 */
extern char *iwram_3001e8c;
extern signed char L371c4[] __asm__(".L371c4");

void Func_801ef68(unsigned short *p, int flags)
{
    char *base;
    unsigned int h1;
    unsigned int w;
    int d;
    int i;
    int n;
    int y;
    unsigned int j;
    unsigned short *q;

    base = iwram_3001e8c;
    n = 1;
    d = 0;
    h1 = p[4] - 1;
    w = p[5];
    if ((flags & 1) == 0)
        flags &= ~2;
    if (flags & 2) {
        d = 5;
        n = 0;
    }
    i = n;
    while (L371c4[i] >= 0) {
        y = L371c4[i] + d;
        if (y < h1) {
            j = 0;
            if (w != 0) {
                do {
                q = (unsigned short *)base + ((p[7] + j) << 5) + (p[6] + y);
                if (j == 0)
                    *q = 0xf018;
                else if (j == w - 1)
                    *q = 0xf019;
                else
                    *q = 0xf00f;
                j++;
                } while (j != w);
            }
        }
        i++;
    }
    if (base[0xea5] != 0) {
        q = (unsigned short *)base + (p[7] + p[5] - 1) * 32 + p[6];
        *q++ = 0xf080;
        for (y = 1; y < h1; y++)
            *q++ = 0xf081;
        *q = 0xf082;
    }
    base[0xea3] = 1;
}
