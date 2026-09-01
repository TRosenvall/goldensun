/* TestCollision (0x080120dc) -- NON-MATCHING.
 * Blocker class: a scaled index parked in r12, the high-register-copy wall.
 *
 * 85 lines against the ROM's 89, and the tail is instruction-for-instruction
 * identical once the four missing lines are accounted for. Two of the four are
 * the wall:
 *
 *     rom    lsl r3, #0x2 / mov r12, r3 / ... / mov r2, r12 / ldr r3, [r4, r2]
 *     ours   lsl r3, #0x2 / ... / ldr r4, [r3, r2]
 *
 * The ROM parks the scaled function-table index in r12 across four
 * instructions, which frees r3 for the function pointer -- hence its
 * `bl _call_via_r3` against our `bl _call_via_r4`. It is register pressure, and
 * the source has no way to ask for it.
 *
 * The other two are `mov r2, r3` (a copy of the tile kind before multiplying by
 * three) and the ewram read, which the ROM splits into
 * `add r0, r1, r3 / ldrb r0, [r0, #0x0]` where we get `ldrb r2, [r0, r3]`.
 *
 * THAT LAST ONE IS THE INTERESTING PART, because the fix is already recorded
 * and it does not work here. src/non_matching/rom_9000/8011f54.c -- a close
 * relative reading the SAME two tables -- needed a named pointer
 * (`q = ewram_202c000 + idx; ... *q ...`) to get the split form, and that
 * spelling matched there. This function uses the identical spelling and gcc
 * folds it back into a register+register load anyway.
 *
 * So the batch-174 addressing discriminator is NOT a spelling rule that always
 * holds. The named pointer only survives when register pressure leaves gcc a
 * register to hold it in. Here pressure is higher -- the same function is also
 * juggling a function pointer, two masked coordinates and the table base -- and
 * gcc folds the address back into the load. **The addressing forms are a
 * PREFERENCE the source can express, not a guarantee**, and when a
 * previously-working spelling goes inert, look at what else is competing for
 * registers before concluding the rule is wrong.
 *
 * MEASURED (rom 89 lines):
 *   `y` read before `x`, as the ROM emits them        85, 81
 *   `x` read before `y` (gcc reverses the pair)       85, 77
 *   `unsigned` tile kind for the `bhi` comparison     85, 76  <- best
 *
 * WHAT IS RIGHT: the named table offset for `ldr r2, [r1, r3]`; the signed
 * `/ 16` expansion; `_call_via_r3` from an ordinary indirect call; the
 * `(0x80 << 12)` and `-0xc0000` bounds; and the `mov r0, #0x2` that gcc hoists
 * above the `0xff` test on its own from a plain `return 2;`.
 *
 * NEXT: nothing source-level. The r12 parking is the wall.
 */
extern unsigned char *iwram_3001e70;
extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];
extern unsigned char ewram_202c001[];
extern unsigned char L134fc[] __asm__(".L134fc");

typedef int (*Fn)(unsigned char *p, int x, int y);

int TestCollision(unsigned char *a, unsigned char *b)
{
    unsigned char *base;
    unsigned char *src;
    unsigned char *p;
    unsigned char *q;
    Fn fn;
    int x;
    int y;
    unsigned int k;
    int off;
    int idx;
    int fnk;
    int d;

    x = *(short *)(b + 2);
    y = *(short *)(b + 0xa);
    base = iwram_3001e70;
    if (base == 0)
        return 0;
    k = a[0x22];
    if (k <= 2) {
        off = (k * 3 << 4) + (0x98 << 1);
        src = *(unsigned char **)(base + off);
    } else {
        src = gBuffer;
    }
    p = src + ((x / 16) + (y / 16) * 128) * 4;
    if (p[2] == 0xff)
        return 2;
    idx = p[3] * 4;
    q = ewram_202c000 + idx;
    fnk = (*q & 0xf) * 4;
    fn = *(Fn *)(L134fc + fnk);
    d = fn(ewram_202c001 + idx, x & 0xf, y & 0xf) - *(int *)(a + 0x14);
    if (d > (0x80 << 12))
        return 1;
    if (d < -0xc0000)
        return -1;
    return 0;
}
