/* OvlFunc_881_200a7dc -- NON-MATCHING.
 * Blocker class: DUPLICATED BASE POINTER, the elided-copy shape.
 * 31 lines against the ROM's 33, TWO SHORT, 32 differing.
 *
 * A scan over 12-byte records for one whose first word is 2 and whose halfword
 * at +4 is 0x8a, stopping at a first word of -1. The ROM carries the table
 * base in TWO registers -- `ldr r0, =gOvl_0200e3f4 / mov r7, r0` -- reading
 * through `[r2, r0]` and writing through `[r2, r7]`. gcc sees one value, keeps
 * one register, and the function comes out two instructions shorter.
 *
 * This is the same shape as Func_80bf54c and the other elided-copy parks: the
 * ROM copies a value before use and gcc loads straight into the destination.
 * Writing two locals both assigned the base does not produce two registers,
 * because they are provably equal.
 *
 * Worth contrasting with Func_80a1bdc, elevated this batch, where adding a
 * SECOND POINTER did work. There the two pointers advance differently -- one
 * post-increments for the read, the other lags for the call argument -- so
 * they hold different values and gcc must keep both. Here they never differ.
 * The lever is "give the source the values the ROM carries" only when those
 * values are actually distinct.
 */
extern char gOvl_0200e3f4[];

void OvlFunc_881_200a7dc(void)
{
    char *base;
    char *w;
    char *p;
    int off;
    int k, one, neg1;

    base = gOvl_0200e3f4;
    k = 0x21;
    neg1 = 1;
    w = base;
    one = 1;
    p = base + 4;
    off = 0;
    neg1 = -neg1;
    for (;;) {
        if (*(int *)(base + off) == 2 && *(short *)p == 0x8a) {
            *(int *)(w + off) = one;
            *(int *)(p + 4) = k;
            return;
        }
        if (*(int *)(base + off) == neg1)
            return;
        p += 0xc;
        off += 0xc;
    }
}
