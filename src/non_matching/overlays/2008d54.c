/*
 * OvlFunc_959_2008d54 -- asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_a.s
 *
 * BLOCKER: register-role swap. 54 lines against 54, 13 differing, and the
 * whole disagreement is that the ROM loads the table's first word into r7 and
 * the second into r6, while we do the reverse. Everything else -- both
 * six-argument calls, the three derived offsets, the conditional fourth call --
 * matches.
 *
 * SETTLED, and this is the transferable part. It took 18 differing to 13:
 *
 *   The ROM reads the two table words with `ldr r7, [r2, r3] / add r3, #4 /
 *   ldr r6, [r2, r3]` -- one base register, an offset that is MUTATED between
 *   the loads. Writing the obvious `t = (int *)(L773c + n * 8); a = t[0];
 *   b = t[1];` gives gcc a single pointer and immediate offsets (`ldr [r3]`,
 *   `ldr [r3, #4]`), which is a different shape entirely.
 *
 *   Keeping the offset in its own int and incrementing it --
 *
 *       off = n * 8;
 *       a = *(int *)(tb + off);
 *       off += 4;
 *       b = *(int *)(tb + off);
 *
 *   -- reproduces the register-offset pair exactly. The `tb` pointer local is
 *   also required; with the array name used directly gcc folds the base and
 *   emits the operands transposed.
 *
 * TRIED AND REJECTED:
 *
 *   * Swapping the declaration order of `a` and `b`, on the theory that
 *     callee-saved assignment follows declaration order. NO CHANGE.
 */
extern unsigned char L773c[] __asm__(".L773c");
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_2008d54(int n)
{
    unsigned char *tb;
    int off;
    int a;
    int b;
    int c;

    tb = L773c;
    off = n * 8;
    a = *(int *)(tb + off);
    off += 4;
    b = *(int *)(tb + off);
    __Func_80105d4(0, 0x4d, 1, 3, a, b);
    __Func_80105d4(1, 0x4d, 1, 1, a + 1, b);
    c = b - 0x2c;
    __Func_8010704(a, b - 0x2d, 1, 1, a, c);
    if (n == 1)
        __Func_8010704(a, c, 1, 1, a, b - 0x2b);
}
