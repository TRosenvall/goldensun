/* OvlFunc_947_2009938 -- 0x02009938  (asm/overlays/rom_7d0e88/ovl_1528_a_a_c.s)
 *
 * TWIN: OvlFunc_947_20099f0 in the same .s is the same shape, so this park
 * covers two functions.
 *
 * BLOCKER: register roles and load order in the byte-field block. 36 of 91,
 * one line short. The entire guard chain -- six early exits, a signed
 * division by 0x10000 with its rounding correction, and both two-sided range
 * tests -- is EXACT.
 *
 * PROGRESSION:
 *   82  naive, with `return 0` at each guard
 *   38  ONE `ret` variable set to 0 at the top, all guards `goto out`, and
 *       `ret = 1` before the label. The ROM sets r0 once BEFORE the first
 *       compare; per-exit returns make gcc materialise it at each one.
 *   36  the two range constants written as SIGNED NEGATIVES  <- best
 *
 * That last step is small and worth keeping: `b[2] + 0xfff00000 >= a[2]`
 * compiles the comparison UNSIGNED (`bcs`) because the literal is a large
 * unsigned constant; `b[2] - 0x100000 >= a[2]` keeps it signed (`bge`) and
 * emits the identical `ldr r2, =0xfff00000 / add`. The pool word is the same
 * either way -- only the branch differs -- so the ROM's `bge` against our
 * `bcs` is a SIGNEDNESS tell about the source literal, not about the operands.
 *
 * WHAT REMAINS is the final block: the ROM loads a's sprite before b's and we
 * do the reverse, and every register downstream follows. Tried and inert:
 * naming the shifted intermediate so the later `lsr #30 / lsl #2` recomputes
 * from it rather than from the extracted field (gcc commons it anyway).
 *
 * THE ALIASING TELL DOES NOT APPLY HERE, and that is the useful negative.
 * This function was selected by tools/aliastell.py for re-reading a[0x50]
 * after a store, and the re-read ALREADY MATCHES without the flag --
 * -fno-strict-aliasing changes nothing at any stage. The store is to a
 * `char` lvalue (a[0x23] &= 0xfe), and a character type aliases everything
 * under the standard, so gcc must reload regardless. The tell needs a store
 * whose type CANNOT alias the reload: short against int is the shape that
 * paid on Func_808d828 and Func_80935d4. A byte store is not it.
 */
int OvlFunc_947_2009938(int *a, int *b)
{
    unsigned char *sa;
    unsigned char *sb;
    unsigned int ka;
    unsigned int kb;
    int m;
    int ret;

    ret = 0;
    if (b[2] == a[2] && b[3] == a[3] && b[4] == a[4])
        goto out;
    if (b[2] - 0x100000 >= a[2])
        goto out;
    if (a[2] >= b[2] + (0x80 << 13))
        goto out;
    if (b[3] / 0x10000 != a[3] / 0x10000)
        goto out;
    if (b[4] <= a[4])
        goto out;
    if (b[4] - 0x200000 >= a[4])
        goto out;
    sa = *(unsigned char **)((char *)a + 0x50);
    sb = *(unsigned char **)((char *)b + 0x50);
    ka = (unsigned int)(sa[9] << 28) >> 30;
    kb = (unsigned int)(sb[9] << 28) >> 30;
    if (ka < kb) {
        ((unsigned char *)a)[0x23] &= 0xfe;
        m = -0xd;
        sa = *(unsigned char **)((char *)a + 0x50);
        sa[9] = (sa[9] & m) | (kb << 2);
        sb = *(unsigned char **)((char *)b + 0x50);
        sa = *(unsigned char **)((char *)a + 0x50);
        sa[0x15] = (sa[0x15] & m) | (sb[0x15] & 0xc);
    }
    ret = 1;
out:
    return ret;
}
