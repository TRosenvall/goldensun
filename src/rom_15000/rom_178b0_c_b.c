/* Func_8017c1c  --  0x08017c1c, split out of asm/rom_15000/rom_178b0_c.s;
 * Func_8017aa4 stays in _a.s.
 *
 * Out of a park (recorded 7, re-measured at 12 differing and 4 bytes short).
 * Two edits:
 *  - Store a literal 0 instead of an `int z` local. The ROM's mid-function
 *    pool, with 0 and 0x1ff ahead of the `.pool`, shows both are narrow
 *    halfword constants, so the zero wants a halfword pool load; the park's
 *    "the zero needs an int local" was backwards here. 12 -> 3.
 *  - Name the third argument: `d = (char *)0x6002000 + n;` then the call.
 */
struct S {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

extern char *iwram_3001e8c;
extern void Func_801de5c(void *a, void *b, void *c);

void Func_8017c1c(void *p, struct S *s, int x, int y)
{
    char *base;
    unsigned short *q;
    int off;
    int n;

    base = iwram_3001e8c;
    if (p == 0) {
        q = (unsigned short *)(base + 0x12b2);
        p = base + (0xeb << 4);
        off = (0xeb << 4) + (*q << 1);
        *(unsigned short *)(base + off) = 0;
        *q = (*q + 1) & 0x1ff;
    }
    n = ((s->fe + y + 1) << 5) + (s->fc + x) + 1;
    if ((unsigned int)n < 0x280) {
        char *d;
        n = n * 2;
        d = (char *)0x6002000 + n;
        Func_801de5c(p, base + n, d);
    }
}
