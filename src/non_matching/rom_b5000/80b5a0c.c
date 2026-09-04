/* Func_80b5a0c -- 0x080b5a0c  [asm/rom_b5000/rom_b5a0c_a_a_a.s]
 *
 * NOT MATCHING. Best 94 lines against 97 (the body below); a `for`-loop variant
 * reaches 96 of 97 but buys the extra two lines with a redundant loop-entry
 * guard, so the do-while form below is the better base. The .s holds this
 * function alone, so no layout work is pending.
 *
 * Builds both sides' combatant lists: fill one from Func_80b6a60, terminate it
 * with 0xff, then fill the other from Func_80b6ae0 -- in order when the mode
 * byte at +0x42 is 0 or 1, otherwise permuted through the signed offsets at
 * .Lc2a10 around the midpoint -- and terminate that one too.
 *
 * WHAT LANDED, and the middle one is the reusable part:
 *
 *  - The list base is `s + 2`, NOT the array address. The ROM keeps `s + 2` in
 *    ip across all three paths and folds 0x64 into the INDEX register
 *    (`strh r2, [r5, r3]` with r3 = 0x64 + 2*idx). Collapsing that to one
 *    pointer `(short *)(s + 0x66)` is TEN INSTRUCTIONS SHORT -- 83 of 97
 *    against 93. Writing `p = (short *)(s + 2)` in each of the three paths and
 *    indexing `p[0x32 + ...]` recovers all ten. When a ROM folds a constant
 *    into the index rather than the base, the BASE is a real variable.
 *  - The returned count needs its own callee-saved register (`mov r6, r0`);
 *    pinning it to r6 is what produces the copy.
 *  - An explicit index into the source buffer rather than `buf[n - i]`.
 *
 * BLOCKER CLASS: induction-variable form in the first copy loop, three lines.
 * The ROM runs THREE induction variables -- a byte offset that walks
 * (`add r4, #2`, used as `ldrh r3, [r4, r0]` against a FIXED buffer base in
 * r0), a walking destination pointer, and a separate down-counter. gcc merges
 * the first two: it strength-reduces the source subscript into a walking
 * pointer (`ldrh r3, [r0, #0x0]` / `add r0, #2`) and keeps no offset register.
 *
 * MEASURED, four spellings of that one loop:
 *
 *     do { *d = buf[j]; d++; j++; i--; } while (i)      94 of 97   <- below
 *     do { *d = buf[n - i]; d++; i--; } while (i)       93 of 97
 *     for (j = 0; j < n; j++) { *d = buf[j]; d++; }     96 of 97, but the two
 *                                extra lines are a loop guard gcc adds because
 *                                the `for` cannot see the enclosing `if (n > 0)`
 *     byte-stepped index, `*(short *)((char *)buf + j)` with `j += 2`   95 of 97
 *
 * All four strength-reduce the source. NEXT: the untried direction is to make
 * the buffer base ineligible for strength reduction -- it is `sp`, which gcc
 * can rematerialise freely, and that is plausibly why it prefers to walk it.
 * Nothing here varied that; every attempt varied the SUBSCRIPT instead, which
 * is one spelling tried four times by docs/elevation.md's own test.
 *
 * The two 0xff terminators are plain literals stored through a cast, which is
 * why they pool -- the narrow-store table's first row, not a linker symbol.
 * Batch 218's OvlFunc_928_2008500 is the reminder that a pooled small constant
 * is not automatically a symbol.
 */
extern unsigned char *iwram_3001e74;
extern signed char Lc2a10[] __asm__(".Lc2a10");
extern int Func_80b6a60(short *buf);
extern int Func_80b6ae0(short *buf);

void Func_80b5a0c(void)
{
    short buf[14];
    unsigned char *s;
    short *p;
    short *d;
    signed char *t;
    short *src;
    register int n __asm__("r6");
    int j;
    int m;
    int i;
    int k;
    int h;

    s = iwram_3001e74;
    n = Func_80b6a60(buf);
    if (n > 0) {
        d = (short *)(s + 0x58);
        i = n;
        j = 0;
        do {
            *d = buf[j];
            d++;
            j++;
            i--;
        } while (i != 0);
    }
    *(short *)(s + 0x58 + n * 2) = 0xff;
    m = Func_80b6ae0(buf);
    k = s[0x42];
    if (k >= 0 && k <= 1) {
        p = (short *)(s + 2);
        if (0 < m) {
            d = (short *)(s + 0x66);
            for (i = 0; i < m; i++) {
                *d = buf[i];
                d++;
            }
        }
    } else if (m > 0) {
        h = m / 2;
        p = (short *)(s + 2);
        t = Lc2a10;
        src = buf;
        i = m;
        do {
            p[0x32 + *t + h] = *src;
            t++;
            src++;
            i--;
        } while (i != 0);
    } else {
        p = (short *)(s + 2);
    }
    p[0x32 + m] = 0xff;
}
